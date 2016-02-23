#include "Rom.h"
#include "Nes.h"
#include "LoadLib.hpp"
#include "IDrawer.h"
#include "NESToRGBA.h"
#include "Ppu.h"
#include "Joypad.h"
#include "Cpu.h"

#include <iostream>

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
	if (!rom.initialize("test.nes"))
		Error::getInstance()->display();
	rom.loadIntoMemory(nes.getCpuMemory(), nes.getPpuMemory());
	while (42) {
		cpu.loop();
		if (ppu.isFrameRendered())
			drawer->update(test2);
	}
	return (0);
}