#include "loadLibWindows.h"

LoadLibWindows::LoadLibWindows() {}

LoadLibWindows::~LoadLibWindows() {}

void	LoadLibWindows::loadLibrary(char const *name)
{
	std::string	realName(name);

	realName += ".dll";
	this->_modul = LoadLibrary(realName.c_str());
}

void*	LoadLibWindows::getLib(char const *name) {
	return (GetProcAddress(this->_modul, name));
}

bool	LoadLibWindows::closeLib() {
	return (FreeLibrary(this->_modul)) ? (true) : (false);
}
