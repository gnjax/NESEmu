#pragma once

class IDrawer
{
public:
	virtual ~IDrawer() {}
	virtual	void	update(char *tiles) = 0;
};

