#pragma once
#include <string.h>

class Nes {
	char*	ram;
	char*	vram;
public:
	Nes();
	~Nes();
	char*	getCpuMemory();
	char	getResetAddress();
	char*	getPpuMemory();
};

