#pragma once

#include		<cstdint>
#include		<cstdlib>
#include		"Nes.h"
#include		"Ppu.h"
#include		"Error.h"
#include		"Joypad.h"

#define	C_FLAG			(this->PS & 0b00000001)
#define	Z_FLAG			(this->PS & 0b00000010)
#define	I_FLAG			(this->PS & 0b00000100)
#define	D_FLAG			(this->PS & 0b00001000)
#define	B_FLAG			(this->PS & 0b00010000)
#define	V_FLAG			(this->PS & 0b01000000)
#define	N_FLAG			(this->PS & 0b10000000)

#define	SET_C_FLAG		(this->PS |= 0b00000001)
#define	SET_Z_FLAG		(this->PS |= 0b00000010)
#define	SET_I_FLAG		(this->PS |= 0b00000100)
#define	SET_D_FLAG		(this->PS |= 0b00001000)
#define	SET_B_FLAG		(this->PS |= 0b00010000)
#define	SET_V_FLAG		(this->PS |= 0b01000000)
#define	SET_N_FLAG		(this->PS |= 0b10000000)

#define	UNSET_C_FLAG	(this->PS &= 0b11111110)
#define	UNSET_Z_FLAG	(this->PS &= 0b11111101)
#define	UNSET_I_FLAG	(this->PS &= 0b11111011)
#define	UNSET_D_FLAG	(this->PS &= 0b11110111)
#define	UNSET_B_FLAG	(this->PS &= 0b11101111)
#define	UNSET_V_FLAG	(this->PS &= 0b10111111)
#define	UNSET_N_FLAG	(this->PS &= 0b01111111)

#define BND_CHK(x, y)	((x & 0xFF00) != ((y + x) & 0xFF00))

#define	SP_OFFSET		(0x0100)

class Cpu {
	Nes*			nes;
	Ppu*			ppu;
	Joypad*			joypad;
	char			A;
	char			X;
	char			Y;
	char			PS;
	unsigned char	SP;
	uint16_t		PC;

	uint16_t		getValue(uint16_t addr, bool wrapZeroPage = false);
	uint16_t		getValuePageWrapped(uint16_t addr);
	void			VC_FlagHandler(unsigned char ans, unsigned char val);
	void			ZNV_FlagHandler(char ans, char val);
	void			ZN_FlagHandler(unsigned char val);

	void			CMP(unsigned char regVal, unsigned char opVal);
	unsigned char	readRAM(uint16_t addr, bool wrapZeroPage = false);
	void			writeRAM(uint16_t addr, unsigned char val, bool wrapZeroPage = false);
	void nmiRoutine();
public:
	Cpu(Nes* nes, Ppu* ppu, Joypad* joypad);
	~Cpu();
	void			setProgramCounter(uint16_t address);
	void			loop(char *);
};

