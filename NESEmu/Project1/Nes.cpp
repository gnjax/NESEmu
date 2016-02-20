#include "Nes.h"

Nes::Nes() {
	this->ram = new char[0x10000]();
	memset(this->ram, 0, 0x10000);
	this->vram = new char[0x4000]();
	memset(this->vram, 0, 0x4000);
}

Nes::~Nes() {
	delete this->ram;
	delete this->vram;
}

char*	Nes::getPpuMemory() {
	return this->vram;
}

char*	Nes::getCpuMemory() {
	return this->ram;
}

char	Nes::getResetAddress() {
	return ((this->ram[0xFFFD] << 8) | this->ram[0xFFFC]);
}