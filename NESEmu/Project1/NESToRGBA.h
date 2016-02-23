#pragma once

#include	<map>
#include	<array>

class NESToRGBA
{
	std::map<uint8_t, std::array<uint8_t, 4>>	_palette;
public:
	NESToRGBA();
	~NESToRGBA();

	void	convert(char* input, char* output, unsigned int size = 61440);
};

