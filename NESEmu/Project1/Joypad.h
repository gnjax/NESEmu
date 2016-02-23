#pragma once

#include		<cstdint>

#include		"Nes.h"

#define			CONTROLLERS_REGONE	(0x4016)
#define			CONTROLLERS_REGTWO	(0x4017)

class Joypad
{
	Nes*		nes;

	unsigned	reg1 : 24;
	unsigned	reg2 : 24;
	bool		latchReg1;
	bool		latchReg2;

public:
	Joypad();
	~Joypad();

	void		readReg(uint16_t addr);
	void		writeReg(uint16_t addr);
	void		update();
};

