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
	PS = (val == 0) ? (PS | 0b00000010) : (PS & 0b11111101);									// set Z_FLAG
	PS = (val < 0) ? (PS | 0b10000000) : (PS & 0b01111111);										// set N_FLAG
	PS = ((val & 0b10000000) != (ans & 0b10000000)) ? (PS | 0b01000000) : (PS & 0b10111111);	// set V_FLAG
}

inline void	Cpu::ZN_FlagHandler(char val) {
	PS = (val == 0) ? (PS | 0b00000010) : (PS & 0b11111101);	// set Z_FLAG
	PS = (val < 0) ? (PS | 0b10000000) : (PS & 0b01111111);		// set N_FLAG
}

inline void	Cpu::CMP(char regVal, char opVal) {
	if (regVal < opVal) {
		SET_N_FLAG;
		UNSET_Z_FLAG;
		UNSET_C_FLAG;
	}
	else if (regVal > opVal) {
		UNSET_N_FLAG;
		UNSET_Z_FLAG;
		SET_C_FLAG;
	}
	else {
		UNSET_N_FLAG;
		SET_Z_FLAG;
		SET_C_FLAG;
	}
}

void	Cpu::loop(char* ram) {
	char ans = A;
	char tmp;
	bool cFlag;

	++PC;
	switch (this->PC - 1) {

		/* ============================ Load and Store Instructions =============================== */

		// -------------- [LDA] Load Accumulator (affected flags: N, Z)
	case 0xA9:	// immediate
		A = ram[PC];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0xA5:	// absolute zero page
		A = ram[ram[PC]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0xB5:	// indexed zero page X
		A = ram[ram[PC + X]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0xAD:	// absolute
		A = ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0xBD:	// absolute indexed X
		A = ram[getValue(PC) + X];
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0xB9:	// absolute indexed Y
		A = ram[getValue(PC) + Y];
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0xA1:	// indexed indirect X
		A = ram[getValue(ram[PC] + X)];
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0xB1:	// indirect indexed Y
		A = ram[getValue(ram[PC]) + Y];
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;

		// -------------- [LDX] Load Register X (affected flags: N, Z)
	case 0xA2:	// immediate
		X = ram[PC];
		++PC;
		ZN_FlagHandler(X);
		break;
	case 0xA6:	// absolute zero page
		X = ram[ram[PC]];
		++PC;
		ZN_FlagHandler(X);
		break;
	case 0xB6:	// indexed zero page Y
		X = ram[ram[PC + Y]];
		++PC;
		ZN_FlagHandler(X);
		break;
	case 0xAE:	// absolute
		X = ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(X);
		break;
	case 0xBE:	// absolute indexed Y
		X = ram[getValue(PC) + Y];
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZN_FlagHandler(X);
		break;

		// -------------- [LDY] Load Register Y (affected flags: N, Z)
	case 0xA0:	// immediate
		Y = ram[PC];
		++PC;
		ZN_FlagHandler(Y);
		break;
	case 0xA4:	// absolute zero page
		Y = ram[ram[PC]];
		++PC;
		ZN_FlagHandler(X);
		break;
	case 0xB4:	// indexed zero page X
		Y = ram[ram[PC + X]];
		++PC;
		ZN_FlagHandler(Y);
		break;
	case 0xAC:	// absolute
		Y = ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(Y);
		break;
	case 0xBC:	// absolute indexed X
		Y = ram[getValue(PC) + Y];
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(Y);
		break;


		// -------------- [STA] Store Accumulator
	case 0x85:	// absolute zero page
		ram[ram[PC]] = A;
		++PC;
		break;
	case 0x95:	// indexed zero page X
		ram[ram[PC + X]] = A;
		++PC;
		break;
	case 0x8D:	// absolute
		ram[getValue(PC)] = A;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		break;
	case 0x9D:	// absolute indexed X
		ram[getValue(PC) + X] = A;
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		break;
	case 0x99:	// absolute indexed Y
		ram[getValue(PC) + Y] = A;
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		break;
	case 0x81:	// indexed indirect X
		ram[getValue(ram[PC] + X)] = A;
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		break;
	case 0x91:	// indirect indexed Y
		ram[getValue(ram[PC]) + Y] = A;
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		break;

		// -------------- [STX] Store X Register
	case 0x86:	// absolute zero page
		ram[ram[PC]] = X;
		++PC;
		break;
	case 0x96:	// indexed zero page Y
		ram[ram[PC + Y]] = X;
		++PC;
		break;
	case 0x8E:	// absolute
		ram[getValue(PC)] = X;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		break;

		// -------------- [STY] Store Y Register
	case 0x84:	// absolute zero page
		ram[ram[PC]] = Y;
		++PC;
		break;
	case 0x94:	// indexed zero page X
		ram[ram[PC + X]] = Y;
		++PC;
		break;
	case 0x8C:	// absolute
		ram[getValue(PC)] = Y;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		break;



		/* ============================ Arithmetic Instructions =================================== */
		
		// -------------- [ADC] Add with Carry (affected flags : N, Z, C, V)
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
	case 0x6D:	// absolute
		A += ram[getValue(PC)] + C_FLAG;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0x7D:	// absolute indexed X
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

		// -------------- [SBC] Sub with Carry (affected flags : N, Z, C, V)
	case 0xE9:	// immediate
		A -= ram[PC] - C_FLAG;
		++PC;
		ZNV_FlagHandler(ans, A);
		break;
	case 0xE5:	// absolute zero page
		A -= ram[ram[PC]] - C_FLAG;
		++PC;
		ZNV_FlagHandler(ans, A);
		break;
	case 0xF5:	// indexed zero page X
		A -= ram[ram[PC + X]] - C_FLAG;
		++PC;
		ZNV_FlagHandler(ans, A);
		break;
	case 0xED:	// absolute
		A -= ram[getValue(PC)] - C_FLAG;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0xFD:	// absolute indexed X
		A -= ram[getValue(PC) + X] - C_FLAG;
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0xF9:	// absolute indexed Y
		A -= ram[getValue(PC) + Y] - C_FLAG;
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0xE1:	// indexed indirect X
		A -= ram[getValue(ram[PC] + X)] - C_FLAG;
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;
	case 0xF1:	// indirect indexed Y
		A -= ram[getValue(ram[PC]) + Y] - C_FLAG;
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		ZNV_FlagHandler(ans, A);
		break;

		
		
		/* ============================ Increment and Decrement Instructions ====================== */
		
		// -------------- [INC] Increment Memory (affected flags: N, Z)
	case 0xE6:	// absolute zero page
		++(ram[ram[PC]]);
		++PC;
		ZN_FlagHandler(ram[ram[PC]]);
		break;
	case 0xF6:	// indexed zero page X
		++(ram[ram[PC] + X]);
		++PC;
		ZN_FlagHandler(ram[ram[PC] + X]);
		break;
	case 0xEE:	// absolute
		++(ram[getValue(PC)]);
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC)]);
		break;
	case 0xFE:	// absolute indexed X
		++(ram[getValue(PC) + X]);
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC) + X]);
		break;

		// -------------- [INX] Increment X (affected flags: N, Z)
	case 0xE8:
		++X;
		ZN_FlagHandler(X);
		break;

		// -------------- [INY] Increment Y (affected flags: N, Z)
	case 0xC8:
		++Y;
		ZN_FlagHandler(Y);
		break;


		// [DEC] Decrement Source (affected flags: N, Z)
	case 0xC6:	// absolute zero page
		--(ram[ram[PC]]);
		++PC;
		ZN_FlagHandler(ram[ram[PC]]);
		break;
	case 0xD6:	// indexed zero page X
		--(ram[ram[PC] + X]);
		++PC;
		ZN_FlagHandler(ram[ram[PC] + X]);
		break;
	case 0xCE:	// absolute
		--(ram[getValue(PC)]);
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC)]);
		break;
	case 0xDE:	// absolute indexed X
		--(ram[getValue(PC) + X]);
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC) + X]);
		break;

		// -------------- [DEX] Decrement X (affected flags: N, Z)
	case 0xCA:
		--X;
		ZN_FlagHandler(X);
		break;

		// -------------- [DEY] Decrement Y (affected flags: N, Z)
	case 0x88:
		--Y;
		ZN_FlagHandler(Y);
		break;

		
		
		/* ============================ Logical Instructions ====================================== */
		
		// -------------- [AND] Logical AND (affected flags : N, Z)
	case 0x29:	// immediate
		A &= ram[PC];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x25:	// absolute zero page
		A &= ram[ram[PC]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x35:	// indexed zero page X
		A &= ram[ram[PC + X]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x2D:	// absolute
		A &= ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x3D:	// absolute indexed X
		A &= ram[getValue(PC) + X];
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x39:	// absolute indexed Y
		A &= ram[getValue(PC) + Y];
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x21:	// indexed indirect X
		A &= ram[getValue(ram[PC] + X)];
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x31:	// indirect indexed Y
		A &= ram[getValue(ram[PC]) + Y];
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;

		// -------------- [ORA] Logical Inclusive OR (affected flags : N, Z)
	case 0x09:	// immediate
		A |= ram[PC];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x05:	// absolute zero page
		A |= ram[ram[PC]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x15:	// indexed zero page X
		A |= ram[ram[PC + X]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x0D:	// absolute
		A |= ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x1D:	// absolute indexed X
		A |= ram[getValue(PC) + X];
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x19:	// absolute indexed Y
		A |= ram[getValue(PC) + Y];
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x01:	// indexed indirect X
		A |= ram[getValue(ram[PC] + X)];
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x11:	// indirect indexed Y
		A |= ram[getValue(ram[PC]) + Y];
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;

		// -------------- [EOR] Exclusive-OR (affected flags : N, Z)
	case 0x49:	// immediate
		A ^= ram[PC];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x45:	// absolute zero page
		A ^= ram[ram[PC]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x55:	// indexed zero page X
		A ^= ram[ram[PC + X]];
		++PC;
		ZN_FlagHandler(A);
		break;
	case 0x4D:	// absolute
		A ^= ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x5D:	// absolute indexed X
		A ^= ram[getValue(PC) + X];
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x59:	// absolute indexed Y
		A ^= ram[getValue(PC) + Y];
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x41:	// indexed indirect X
		A ^= ram[getValue(ram[PC] + X)];
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		ZN_FlagHandler(A);
		break;
	case 0x51:	// indirect indexed Y
		A ^= ram[getValue(ram[PC]) + Y];
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		ZN_FlagHandler(A);
		break;

		
		
		/* ============================ Jump, Branch, Compare, and Test Bits ====================== */
		
		// -------------- [JMP] Jump
	case 0x4c:
		PC = getValue(PC);
		break;
	case 0x6c:
		PC = getValue(getValue(PC));
		break;


		// -------------- [BCC] Branch if Carry Clear
	case 0x90:	// relativ
		if (C_FLAG == 0) {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		else
			++PC;
		break;

		// -------------- [BCS] Branch if Carry Set
	case 0xB0:	// relativ
		if (C_FLAG == 0)
			++PC;
		else {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		break;

		// -------------- [BEQ] Branch if Equal
	case 0xF0:	// relativ
		if (Z_FLAG == 0)
			++PC;
		else {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		break;

		// -------------- [BNE] Branch if not Equal
	case 0xD0:	// relativ
		if (Z_FLAG == 0)
		{
			--PC;
			PC += (char)ram[ram[PC]];
		}
		else
			++PC;
		break;

		// -------------- [BMI] Branch if Minus
	case 0x30:	// relativ
		if (N_FLAG == 0)
			++PC;
		else {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		break;

		// -------------- [BPL] Branch on Plus
	case 0x10:	// relativ
		if (N_FLAG == 0)
		{
			--PC;
			PC += (char)ram[ram[PC]];
		}
		else
			++PC;
		break;
		
		// -------------- [BVS] Branch if Overflow Set
	case 0x70: // relativ
		if (V_FLAG == 0)
			++PC;
		else {
			--PC;
			PC += (char)ram[ram[PC]];
		}
		break;

		// -------------- [BVC] Branch if Overflow Clear
	case 0x50: // relativ
		if (V_FLAG == 0)
		{
			--PC;
			PC += (char)ram[ram[PC]];
		}
		else
			++PC;
		break;


		// -------------- [CMP] Compare Memory And Accumulator (affected flags: N, Z, C)
	case 0xC9:	// immediate
		CMP(A, ram[PC]);
		++PC;
		break;
	case 0xC5:	// absolute zero page
		CMP(A, ram[ram[PC]]);
		++PC;
		break;
	case 0xD5:	// indexed zero page X
		CMP(A, ram[ram[PC + X]]);
		++PC;
		break;
	case 0xCD:	// absolute
		CMP(A, ram[getValue(PC)]);
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		break;
	case 0xDD:	// absolute indexed X
		CMP(A, ram[getValue(PC) + X]);
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		break;
	case 0xD9:	// absolute indexed Y
		CMP(A, ram[getValue(PC) + Y]);
		PpuRegisterObserver(getValue(PC) + Y);
		PC += 2;
		break;
	case 0xC1:	// indexed indirect X
		CMP(A, ram[getValue(ram[PC] + X)]);
		PpuRegisterObserver(getValue(ram[PC] + X));
		PC += 2;
		break;
	case 0xD1:	// indirect indexed Y
		CMP(A, ram[getValue(ram[PC]) + Y]);
		PpuRegisterObserver(getValue(ram[PC]) + Y);
		PC += 2;
		break;

		// -------------- [CPX] Compare Memory And X (affected flags: N, Z, C)
	case 0xE0:	// immediate
		CMP(X, ram[PC]);
		++PC;
		break;
	case 0xE4:	// absolute zero page
		CMP(X, ram[ram[PC]]);
		++PC;
		break;
	case 0xEC:	// absolute
		CMP(X, ram[getValue(PC)]);
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		break;

		// -------------- [CPY] Compare Memory And Y (affected flags: N, Z, C)
	case 0xC0:	// immediate
		CMP(Y, ram[PC]);
		++PC;
		break;
	case 0xC4:	// absolute zero page
		CMP(Y, ram[ram[PC]]);
		++PC;
		break;
	case 0xCC:	// absolute
		CMP(Y, ram[getValue(PC)]);
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		break;


		// -------------- [BIT] Bit Test (affected flag: N, V, Z)
	case 0x24:	// absolute zero page
		tmp = A & ram[ram[PC]];
		++PC;
		ZNV_FlagHandler(ans, tmp);
		break;
	case 0x2C:	// absolute
		tmp = A & ram[getValue(PC)];
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZNV_FlagHandler(ans, tmp);
		break;

		

		/* ============================ Shift and Rotate Instructions ============================= */
		
		// -------------- [ASL] Arithmetic Shift Left (affected flags : N, C, Z)
	case 0x0A:	// A
		PS = (A < 0) ? (PS | 0b00000001) : (PS & 0b11111110); // set C_FLAG
		A <<= 1;
		ZN_FlagHandler(A);
		break;
	case 0x06:	// absolute zero page
		PS = (ram[ram[PC]] < 0) ? (PS | 0b00000001) : (PS & 0b11111110); // set C_FLAG
		ram[ram[PC]] <<= 1;
		++PC;
		ZN_FlagHandler(ram[ram[PC]]);
		break;
	case 0x16:	// indexed zero page X
		PS = (ram[ram[PC] + X] < 0) ? (PS | 0b00000001) : (PS & 0b11111110); // set C_FLAG
		ram[ram[PC] + X] <<= 1;
		++PC;
		ZN_FlagHandler(ram[ram[PC] + X]);
		break;
	case 0x0E:	// absolute
		PS = (ram[getValue(PC)] < 0) ? (PS | 0b00000001) : (PS & 0b11111110); // set C_FLAG
		ram[getValue(PC)] <<= 1;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC)]);
		break;
	case 0x1E:	// absolute indexed X
		PS = (ram[getValue(PC) + X] < 0) ? (PS | 0b00000001) : (PS & 0b11111110); // set C_FLAG
		ram[getValue(PC) + X] <<= 1;
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC) + X]);
		break;

		// -------------- [LSR] Logical Shift Right (affected flags : N, C, Z)
	case 0x4A:	// A
		PS = (A < 0) ? (PS | 0b00000001) : (PS & 0b11111110);	// set C_FLAG
		A >>= 1;
		ZN_FlagHandler(A);
		break;
	case 0x46:	// absolute zero page
		ram[ram[PC]] >>= 1;
		++PC;
		ZN_FlagHandler(ram[ram[PC]]);
		break;
	case 0x56:	// indexed zero page X
		ram[ram[PC] + X] >>= 1;
		++PC;
		ZN_FlagHandler(ram[ram[PC] + X]);
		break;
	case 0x4E:	// absolute
		ram[getValue(PC)] >>= 1;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC)]);
		break;
	case 0x5E:	// absolute indexed X
		ram[getValue(PC) + X] >>= 1;
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC) + X]);
		break;

		// -------------- [ROL] Rotate Left (affected flags : N, C, Z)
	case 0x2A:	// A
		cFlag = (A < 0);
		A <<= 1;
		A |= C_FLAG;
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		ZN_FlagHandler(A);
		break;
	case 0x26:	// absolute zero page
		cFlag = (ram[ram[PC]] < 0);
		ram[ram[PC]] <<= 1;
		ram[ram[PC]] |= C_FLAG;
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		++PC;
		ZN_FlagHandler(ram[ram[PC]]);
		break;
	case 0x36:	// indexed zero page X
		cFlag = (ram[ram[PC] + X] < 0);
		ram[ram[PC] + X] <<= 1;
		ram[ram[PC] + X] |= C_FLAG;
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		++PC;
		ZN_FlagHandler(ram[ram[PC] + X]);
		break;
	case 0x2E:	// absolute
		cFlag = (ram[getValue(PC)] < 0);
		ram[getValue(PC)] <<= 1;
		ram[getValue(PC)] |= C_FLAG;
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC)]);
		break;
	case 0x3E:	// absolute indexed X
		cFlag = (ram[getValue(PC) + X] < 0);
		ram[getValue(PC) + X] <<= 1;
		ram[getValue(PC) + X] |= C_FLAG;
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC) + X]);
		break;

		// -------------- [ROR] Rotate Right (affected flags : N, C, Z)
	case 0x6A:	// A
		cFlag = (A < 0);
		A >>= 1;
		A |= (C_FLAG << 7);
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		ZN_FlagHandler(A);
		break;
	case 0x66:	// absolute zero page
		cFlag = (ram[ram[PC]] < 0);
		ram[ram[PC]] >>= 1;
		ram[ram[PC]] |= (C_FLAG << 7);
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		++PC;
		ZN_FlagHandler(ram[ram[PC]]);
		break;
	case 0x76:	// indexed zero page X
		cFlag = (ram[ram[PC] + X] < 0);
		ram[ram[PC] + X] >>= 1;
		ram[ram[PC] + X] |= (C_FLAG << 7);
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		++PC;
		ZN_FlagHandler(ram[ram[PC] + X]);
		break;
	case 0x6E:	// absolute
		cFlag = (ram[getValue(PC)] < 0);
		ram[getValue(PC)] >>= 1;
		ram[getValue(PC)] |= (C_FLAG << 7);
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		PpuRegisterObserver(getValue(PC));
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC)]);
		break;
	case 0x7E:	// absolute indexed X
		cFlag = (ram[getValue(PC) + X] < 0);
		ram[getValue(PC) + X] >>= 1;
		ram[getValue(PC) + X] |= (C_FLAG << 7);
		if (cFlag)
			SET_C_FLAG;
		else
			UNSET_C_FLAG;
		PpuRegisterObserver(getValue(PC) + X);
		PC += 2;
		ZN_FlagHandler(ram[getValue(PC) + X]);
		break;

		
		
		/* ============================ Transfer Instructions ===================================== */
		
		// -------------- [TAX] Transfer Accumulator to X(affected flags: N, Z)
	case 0xAA:
		X = A;
		ZN_FlagHandler(A);
		break;

		// -------------- [TAY] Transfer Accumulator to Y (affected flags: N, Z)
	case 0xA8:
		Y = A;
		ZN_FlagHandler(A);
		break;

		// -------------- [TXA] Transfer X to Accumulator (affected flags: N, Z)
	case 0x8A:
		A = X;
		ZN_FlagHandler(X);
		break;

		// -------------- [TYA] Transfer Y to Accumulator (affected flags: N, Z)
	case 0x98:
		A = Y;
		ZN_FlagHandler(Y);
		break;

		
		
		/* ============================ Stack Instructions ======================================== */
		
		// -------------- [TSX] Transfer Stack pointer to X (affected flags: N, Z)
	case 0xBA:
		X = SP;
		ZN_FlagHandler(X);
		break;

		// -------------- [TXS] Transfer X to Stack pointer (affected flags: N, Z)
	case 0x9A:
		SP = X;
		ZN_FlagHandler(SP);
		break;


		// -------------- [PHA] Push Accumulator on stack
	case 0x48:
		ram[SP++] = A;
		break;

		// -------------- [PHP] Push Processor Status on stack
	case 0x08:
		ram[SP++] = PS;
		break;

		// -------------- [PLA] Pull Accumulator from stack (affected flags : N, Z)
	case 0x68:
		A = ram[SP--];
		ZN_FlagHandler(A);
		break;

		// -------------- [PLP] Pull Processor Status from stack (affected flags : ALL)
	case 0x28:
		PS = ram[SP--];
		break;

		
		
		/* ============================ Subroutine Instructions =================================== */
		
		// -------------- [JSR] Jump to Subroutine
	case 0x20:
		ram[SP++] = PC & 0x0F;
		ram[SP++] = PC >> 8;
		PC = getValue(PC);
		break;


		// -------------- [RTS]
	case 0x60:
		PC = 0;
		PC |= (ram[SP--] >> 8);
		PC |= ram[SP--];

		// -------------- [RTI]
	case 0x40:
		// TODO
		break;

		

		/* ============================ Set and Reset (Clear) Instructions ======================== */
		
		// -------------- [CLC] Clear Carry (affected flags: C)
	case 0x18:
		UNSET_C_FLAG;
		break;

		// -------------- [CLD] Clear Decimal Mode (affected flags: D)
	case 0xD8:
		UNSET_D_FLAG;
		break;

		// -------------- [CLI] Clear Interrupt Disable (affected flags: I)
	case 0x58:
		UNSET_I_FLAG;
		break;

		// -------------- [CLV] Clear Overflow Flag (affected flags: V)
	case 0xB8:
		UNSET_V_FLAG;
		break;


		// -------------- [SEC] Set Carry Flag (affected flags: C)
	case 0x38:
		SET_C_FLAG;
		break;

		// -------------- [SED] Set Decimal Mode (affected flags: D)
	case 0xF8:
		SET_D_FLAG;
		break;

		// -------------- [SEI] Set Interrupt Disable (affected flags: I)
	case 0x78:
		SET_I_FLAG;
		break;

		
		
		/* ============================ Other Instructions ======================================== */

		// -------------- [NOP] No Opération
	case 0xEA:
		break;		

		// -------------- [BRK] Break (affected flags: B, I)
	case 0x00:
		// TODO : IRQ interupt
		SET_B_FLAG;
		SET_I_FLAG;
		break;

	}
}