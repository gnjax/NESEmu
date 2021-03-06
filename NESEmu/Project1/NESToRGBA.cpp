#include "NESToRGBA.h"



NESToRGBA::NESToRGBA()
{
	this->_palette = {
		{ 0x00,{ 0x80,0x80,0x80,0xFF } },
		{ 0x01,{ 0x00,0x3D,0xA6,0xFF } },
		{ 0x02,{ 0x00,0x12,0xB0,0xFF } },
		{ 0x03,{ 0x44,0x00,0x96,0xFF } },
		{ 0x04,{ 0xA1,0x00,0x5E,0xFF } },
		{ 0x05,{ 0xC7,0x00,0x28,0xFF } },
		{ 0x06,{ 0xBA,0x06,0x00,0xFF } },
		{ 0x07,{ 0x8C,0x17,0x00,0xFF } },
		{ 0x08,{ 0x5C,0x2F,0x00,0xFF } },
		{ 0x09,{ 0x10,0x45,0x00,0xFF } },
		{ 0x0A,{ 0x05,0x4A,0x00,0xFF } },
		{ 0x0B,{ 0x00,0x47,0x2E,0xFF } },
		{ 0x0C,{ 0x00,0x41,0x66,0xFF } },
		{ 0x0D,{ 0x00,0x00,0x00,0xFF } },
		{ 0x0E,{ 0x05,0x05,0x05,0xFF } },
		{ 0x0F,{ 0x05,0x05,0x05,0xFF } },
		{ 0x10,{ 0xC7,0xC7,0xC7,0xFF } },
		{ 0x11,{ 0x00,0x77,0xFF,0xFF } },
		{ 0x12,{ 0x21,0x55,0xFF,0xFF } },
		{ 0x13,{ 0x82,0x37,0xFA,0xFF } },
		{ 0x14,{ 0xEB,0x2F,0xB5,0xFF } },
		{ 0x15,{ 0xFF,0x29,0x50,0xFF } },
		{ 0x16,{ 0xFF,0x22,0x00,0xFF } },
		{ 0x17,{ 0xD6,0x32,0x00,0xFF } },
		{ 0x18,{ 0xC4,0x62,0x00,0xFF } },
		{ 0x19,{ 0x35,0x80,0x00,0xFF } },
		{ 0x1A,{ 0x05,0x8F,0x00,0xFF } },
		{ 0x1B,{ 0x00,0x8A,0x55,0xFF } },
		{ 0x1C,{ 0x00,0x99,0xCC,0xFF } },
		{ 0x1D,{ 0x21,0x21,0x21,0xFF } },
		{ 0x1E,{ 0x09,0x09,0x09,0xFF } },
		{ 0x1F,{ 0x09,0x09,0x09,0xFF } },
		{ 0x20,{ 0xFF,0xFF,0xFF,0xFF } },
		{ 0x21,{ 0x0F,0xD7,0xFF,0xFF } },
		{ 0x22,{ 0x69,0xA2,0xFF,0xFF } },
		{ 0x23,{ 0xD4,0x80,0xFF,0xFF } },
		{ 0x24,{ 0xFF,0x45,0xF3,0xFF } },
		{ 0x25,{ 0xFF,0x61,0x8B,0xFF } },
		{ 0x26,{ 0xFF,0x88,0x33,0xFF } },
		{ 0x27,{ 0xFF,0x9C,0x12,0xFF } },
		{ 0x28,{ 0xFA,0xBC,0x20,0xFF } },
		{ 0x29,{ 0x9F,0xE3,0x0E,0xFF } },
		{ 0x2A,{ 0x2B,0xF0,0x35,0xFF } },
		{ 0x2B,{ 0x0C,0xF0,0xA4,0xFF } },
		{ 0x2C,{ 0x05,0xFB,0xFF,0xFF } },
		{ 0x2D,{ 0x5E,0x5E,0x5E,0xFF } },
		{ 0x2E,{ 0x0D,0x0D,0x0D,0xFF } },
		{ 0x2F,{ 0x0D,0x0D,0x0D,0xFF } },
		{ 0x30,{ 0xFF,0xFF,0xFF,0xFF } },
		{ 0x31,{ 0xA6,0xFC,0xFF,0xFF } },
		{ 0x32,{ 0xB3,0xEC,0xFF,0xFF } },
		{ 0x33,{ 0xDA,0xAB,0xEB,0xFF } },
		{ 0x34,{ 0xFF,0xA8,0xF9,0xFF } },
		{ 0x35,{ 0xFF,0xAB,0xB3,0xFF } },
		{ 0x36,{ 0xFF,0xD2,0xB0,0xFF } },
		{ 0x37,{ 0xFF,0xEF,0xA6,0xFF } },
		{ 0x38,{ 0xFF,0xF7,0x9C,0xFF } },
		{ 0x39,{ 0xD7,0xE8,0x95,0xFF } },
		{ 0x3A,{ 0xA6,0xED,0xAF,0xFF } },
		{ 0x3B,{ 0xA2,0xF2,0xDA,0xFF } },
		{ 0x3C,{ 0x99,0xFF,0xFC,0xFF } },
		{ 0x3D,{ 0xDD,0xDD,0xDD,0xFF } },
		{ 0x3E,{ 0x11,0x11,0x11,0xFF } },
		{ 0x3F,{ 0x11,0x11,0x11,0xFF } },
	};
}


NESToRGBA::~NESToRGBA()
{
}

void	NESToRGBA::convert(char* input, char* output, unsigned int size) {
	for (unsigned int i = 0; i < size; ++i)
		memcpy(output + i * 4, &(this->_palette[input[i]]), 4);
}
