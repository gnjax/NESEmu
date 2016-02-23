#include <iostream>
#include "Drawer.h"

extern "C"
#ifdef _WIN32
	__declspec(dllexport)
#endif
IDrawer*	getInstance() {
	IDrawer* drawer = new Drawer();
	return drawer;
}

void Drawer::_draw(sf::Uint8* tiles)
{
	this->_screen.create(256, 240, tiles);
	this->_texture.loadFromImage(this->_screen);
	this->_sprite.setTexture(this->_texture, true);
	this->_window.draw(this->_sprite);
}

Drawer::Drawer()
{
	this->_window.create(sf::VideoMode(256, 240), "NES Emu");
}

Drawer::~Drawer() {}

void	Drawer::update(char *tiles)
{
	if (this->_window.pollEvent(this->_event)) {
		if (this->_event.type == sf::Event::Closed)
			this->_window.close();
	}
	this->_window.clear();
	this->_draw((sf::Uint8 *)tiles);
	this->_window.display();
}


