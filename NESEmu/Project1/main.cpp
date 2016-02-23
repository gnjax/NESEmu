#include "Rom.h"
#include "Nes.h"
#include "LoadLib.hpp"
#include "IDrawer.h"
#include "NESToRGBA.h"

#include <iostream>

int		main() {
	LoadLib<> libloader;
	libloader.loadLibrary("SFML");
	IDrawer* drawer = libloader.getInstance<IDrawer>();

	NESToRGBA	colorConverter;
	char* test = new char[61440];
	char* test2 = new char[61440 * 4];

	for (int i = 0; i < 61440; ++i)
		test[i] = ((i / (256 * 3)) % 0x3F);

	colorConverter.convert(test, &test2);

	while (42)
		drawer->update(test2);
	return (0);
}