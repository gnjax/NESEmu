#pragma once

#include <SFML/Graphics.hpp>
#include "../Project1/IDrawer.h"

class Drawer :	public IDrawer
{
	sf::RenderWindow	_window;
	sf::Image			_screen;
	sf::Texture			_texture;
	sf::Sprite			_sprite;
	sf::Event			_event;
	sf::Clock			_clock;

	void				_draw(sf::Uint8* tiles);
public:
	Drawer();
	~Drawer();

	void	update(char* tiles);
};

