#include "Joypad.h"



Joypad::Joypad()
{
}


Joypad::~Joypad()
{
}

void Joypad::readReg(uint16_t addr) {
	/*if (val == 0x01) {
		if (addr == CONTROLLERS_REGONE && latchReg1)
			update();
	}
	if (val == 0x00) {
		if (addr == CONTROLLERS_REGONE)
			latchReg1 = true;
		else
			latchReg2 = true;
	}*/
}

void Joypad::writeReg(uint16_t addr)
{
}

void Joypad::update()
{
}
