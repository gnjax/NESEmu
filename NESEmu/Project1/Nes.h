#pragma once
#include <string.h>

class Nes {
	char*	memory;
public:
	Nes();
	~Nes();
	char*	getMemory();
};

