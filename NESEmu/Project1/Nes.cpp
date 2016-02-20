#include "Nes.h"

Nes::Nes() {
	this->memory = new char[0x10000]();
	memset(this->memory, 0, 0x10000);
}

Nes::~Nes() {
	delete this->memory;
}

char*	Nes::getMemory() {
	return this->memory;
}