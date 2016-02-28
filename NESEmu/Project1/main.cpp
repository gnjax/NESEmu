#include "Rom.h"
#include "Nes.h"
#include "LoadLib.hpp"
#include "IDrawer.h"
#include "NESToRGBA.h"
#include "Ppu.h"
#include "Joypad.h"
#include "Cpu.h"

#include <iostream>

#include <fstream>

int		main() {
	LoadLib<> libloader;
	libloader.loadLibrary("SFML");
	IDrawer* drawer = libloader.getInstance<IDrawer>();
	char* test2 = new char[61440 * 4];
	Nes		nes;
	Rom		rom;
	Joypad	joypad;
	Ppu		ppu(nes.getPpuMemory(), nes.getCpuMemory(), test2, rom.getMirroring());
	Cpu		cpu(&nes, &ppu, &joypad);
	if (!rom.initialize("C:\\Qt\\test.nes"))
		Error::getInstance()->display();
	rom.loadIntoMemory(nes.getCpuMemory(), nes.getPpuMemory());
	cpu.setProgramCounter(nes.getResetAddress());
	printf("%x\n", nes.getResetAddress());
	//cpu.setProgramCounter(nes.getResetAddress());

	// create a file-reading object
	std::ifstream fin;
	fin.open("C:\\Qt\\nestest.log"); // open a file
	
	if (!fin.good())
		return 1; // exit if file not found

				  // read each line of the file
	while (!fin.eof())
	{
		char buf[1024];
		char tmp[1024];
		fin.getline(buf, 1024);

		int n = 0;
		char* context = NULL;
		const char* token[128] = {};

		token[0] = strtok_s(buf, " ", &context);
		token[1] = strtok_s(NULL, " ", &context);
		strtok_s(NULL, ":", &context);
		token[2] = strtok_s(NULL, " ", &context);
		strtok_s(NULL, ":", &context);
		token[3] = strtok_s(NULL, " ", &context);
		strtok_s(NULL, ":", &context);
		token[4] = strtok_s(NULL, " ", &context);
		strtok_s(NULL, ":", &context);
		token[5] = strtok_s(NULL, " ", &context);
		strtok_s(NULL, ":", &context);
		token[6] = strtok_s(NULL, " ", &context);
		strtok_s(NULL, ":", &context);
		token[7] = strtok_s(NULL, " ", &context);
		strtok_s(NULL, ":", &context);
		token[8] = strtok_s(NULL, " ", &context);
		sprintf_s(tmp, "%s %s\t\tA:%s X:%s Y:%s P:%s SP:%s CYC:%s SL:%s", token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8]);
		cpu.loop(tmp);
	}
	
	/*while (42) {
		cpu.loop(NULL);
		if (ppu.isFrameRendered())
			drawer->update(test2);
	}*/
	return (0);
}