#include "Cpu.h"

Cpu::Cpu() {
	this->accumulator = 0;
	this->xRegister = 0;
	this->yRegister = 0;
	this->PS = 0;
	this->SP = 0x100;
}

Cpu::~Cpu() {
}

void	Cpu::setProgramCounter(char address) {
	this->PC = address;
}
