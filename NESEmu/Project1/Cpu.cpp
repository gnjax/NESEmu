#include "Cpu.h"

Cpu::Cpu(Nes* nes) {
	this->nes = nes;
	this->ram = (unsigned char *) ram;
	this->A = 0;
	this->X = 0;
	this->Y = 0;
	this->PS = 0;
	this->SP = 0x100;
}

Cpu::~Cpu() {
}

void	Cpu::setProgramCounter(char address) {
	this->PC = address;
}

void	Cpu::PpuRegisterObserver(uint16_t addr) {

}

inline unsigned __int16	Cpu::getValue(uint16_t addr) {
	return (ram[addr + 1] << 8) | ram[addr];
}

inline void	Cpu::ZNV_FlagHandler(char ans, char val) {
	PS = (val == 0) ? (PS | 0b00000010) : (PS & 0b11111101);								// set Z_FLAG
	PS = (val < 0) ? (PS | 0b10000000) : (PS & 0b01111111);									// set N_FLAG
	PS = (val & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);	// set V_FLAG
}

inline void	Cpu::ZN_FlagHandler(char ans, char val) {
	PS = (val == 0) ? (PS | 0b00000010) : (PS & 0b11111101);								// set Z_FLAG
	PS = (val < 0) ? (PS | 0b10000000) : (PS & 0b01111111);									// set N_FLAG
}

void	Cpu::loop(char* ram) {
	char ans = A;

	++PC;
	switch (this->PC - 1) {

		// [ADC] affected flags : N, Z, C, V 
	case 0x69:	// immediate
		A += ram[PC] + C_FLAG;
		++PC;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x65:	// absolute zero page
		A += ram[ram[PC]] + C_FLAG;
		++PC;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x75:	// indexed zero page X
		A += ram[ram[PC + X]] + C_FLAG;
		++PC;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x60:	// absolute
		A += ram[getValue(PC)] + C_FLAG;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x70:	// absolute indexed X
		A += ram[getValue(PC) + X] + C_FLAG;
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x79:	// absolute indexed Y
		A += ram[getValue(PC) + Y] + C_FLAG;
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x61:	// indexed indirect X
		A += ram[getValue(ram[PC] + X)] + C_FLAG;
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x71:	// indirect indexed Y
		A += ram[getValue(ram[PC]) + Y] + C_FLAG;
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;

		// [AND] affected flags : N, Z 
	case 0x29:	// immediate
		A &= ram[PC];
		++PC;
		ZN_FlagHandler(ans, A);
		break;
	case 0x25:	// absolute zero page
		A &= ram[ram[PC]];
		++PC;
		ZN_FlagHandler(ans, A);
		break;
	case 0x35:	// indexed zero page X
		A &= ram[ram[PC + X]];
		++PC;
		ZN_FlagHandler(ans, A);
		break;
	case 0x2D:	// absolute
		A &= ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(ans, A);
		break;
	case 0x3D:	// absolute indexed X
		A &= ram[getValue(PC) + X];
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(ans, A);
		break;
	case 0x39:	// absolute indexed Y
		A &= ram[getValue(PC) + Y];
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZN_FlagHandler(ans, A);
		break;
	case 0x21:	// indexed indirect X
		A &= ram[getValue(ram[PC] + X)];
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		ZN_FlagHandler(ans, A);
		break;
	case 0x31:	// indirect indexed Y
		A &= ram[getValue(ram[PC]) + Y];
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		ZN_FlagHandler(ans, A);
		break;

		// [ASL] affected flags : N, C, Z 
	case 0x0A:	// A
		PS = (A < 0) ? (PS | 0b00000001) : (PS & 0b11111110);	// set C_FLAG
		A << 1;
		ZN_FlagHandler(ans, A);
		break;
	case 0x06:	// absolute zero page
		ram[ram[PC]] << 1;
		++PC;
		ZN_FlagHandler(ans, ram[ram[PC]]);
		break;
	case 0x16:	// indexed zero page X
		ram[ram[PC] + X] << 1;
		++PC;
		ZN_FlagHandler(ans, ram[ram[PC] + X]);
		break;
	case 0x0E:	// absolute
		ram[getValue(PC)] << 1;
		PC += 2;
		ZN_FlagHandler(ans, ram[getValue(PC)]);
		break;
	case 0x1E:	// absolute indexed X
		ram[getValue(PC) + X] << 1;
		PC += 2;
		ZN_FlagHandler(ans, ram[getValue(PC) + X]);
		break;

		// [BCC]
	case 0x90:	// relativ
		if (C_FLAG == 0) {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		else
			++PC;
		break;

		// [BCS]
	case 0xB0:	// relativ
		if (C_FLAG == 0)
			++PC;
		else {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		break;

	// [BEQ]
	case 0xF0:	// relativ
		if (Z_FLAG == 0)
			++PC;
		else {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		break;
	}
}