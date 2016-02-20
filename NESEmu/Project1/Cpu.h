#pragma once

class Cpu {
	char	accumulator;
	char	xRegister;
	char	yRegister;
	char	SP;
	char	PS;
	__int16	PC;
public:
	Cpu();
	~Cpu();
	void	setProgramCounter(char address);
};

