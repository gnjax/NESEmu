#pragma once

#include		<cstdint>
#include		"Nes.h"

#define	C_FLAG	(this->PS & 0b00000001)
#define	Z_FLAG	(this->PS & 0b00000010)
#define	I_FLAG	(this->PS & 0b00000100)
#define	D_FLAG	(this->PS & 0b00001000)
#define	B_FLAG	(this->PS & 0b00010000)
#define	V_FLAG	(this->PS & 0b01000000)
#define	N_FLAG	(this->PS & 0b10000000)

class Cpu {
	Nes*		nes;
	char		A;
	char		X;
	char		Y;
	char		SP;
	char		PS;
	uint16_t	PC;

	uint16_t	getValue(uint16_t addr);
public:
	Cpu(Nes* nes);
	~Cpu();
	void		setProgramCounter(char address);
	void		loop(char* memory);
};

