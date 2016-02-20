#include "Cpu.h"

Cpu::Cpu(Nes* nes) {
	this->nes = nes;
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

inline unsigned __int16	Cpu::getValue(uint16_t addr) {
	return (this->nes->getCpuMemory()[addr + 1] << 8) | this->nes->getCpuMemory()[addr];
}

void	Cpu::loop(char* ram) {
	char ans = A;

	++PC;
	switch (this->PC - 1) {

	// [ADC] affected flags : N, Z, C, V 
	case 0x69: // immediate
		
		A += ram[PC] + C_FLAG;
		++PC;
		PS = (A == 0) ? (PS | 0b00000010) : (PS & 0b11111101);
		PS = (A < 0) ? (PS | 0b10000000) : (PS & 0b01111111);
		PS = (A & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);
		break;
	case 0x65: // zero page
		A += ram[ram[PC]] + C_FLAG;
		++PC;
		break;
	case 0x75: // indexed zero page X
		A += ram[ram[PC + X]] + C_FLAG;
		++PC;
		PS = (A == 0) ? (PS | 0b00000010) : (PS & 0b11111101);
		PS = (A < 0) ? (PS | 0b10000000) : (PS & 0b01111111);
		PS = (A & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);
		break;
	case 0x60: // absolute
		A += getValue(ram[PC]) + C_FLAG;
		PC += 2;
		PS = (A == 0) ? (PS | 0b00000010) : (PS & 0b11111101);
		PS = (A < 0) ? (PS | 0b10000000) : (PS & 0b01111111);
		PS = (A & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);
		break;
	case 0x70: // indexed X
		A += getValue(ram[PC + X]) + C_FLAG;
		PC += 2;
		PS = (A == 0) ? (PS | 0b00000010) : (PS & 0b11111101);
		PS = (A < 0) ? (PS | 0b10000000) : (PS & 0b01111111);
		PS = (A & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);
		break;
	case 0x79: // indexed Y
		A += getValue(ram[PC + Y]) + C_FLAG;
		PC += 2;
		PS = (A == 0) ? (PS | 0b00000010) : (PS & 0b11111101);
		PS = (A < 0) ? (PS | 0b10000000) : (PS & 0b01111111);
		PS = (A & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);
		break;
	case 0x61: // indexed indirect X
		A += getValue(getValue(ram[PC + X])) + C_FLAG;
		PC += 2;
		PS = (A == 0) ? (PS | 0b00000010) : (PS & 0b11111101);
		PS = (A < 0) ? (PS | 0b10000000) : (PS & 0b01111111);
		PS = (A & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);
		break;
	case 0x71: // indirect indexed Y
		A += getValue(getValue(ram[PC]) + Y) + C_FLAG;
		PC += 2;
		PS = (A == 0) ? (PS | 0b00000010) : (PS & 0b11111101);
		PS = (A < 0) ? (PS | 0b10000000) : (PS & 0b01111111);
		PS = (A & 0b10000000 != ans & 0b10000000) ? (PS | 0b01000000) : (PS & 0b10111111);
		break;
	}
}