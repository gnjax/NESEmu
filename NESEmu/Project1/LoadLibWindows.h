#pragma once

#include		<Windows.h>
#include		<string>

class			LoadLibWindows
{
private:
	HMODULE		_modul;
public:
	LoadLibWindows();
	~LoadLibWindows();
	void		loadLibrary(char const *);
	void*		getLib(char const *);
	bool		closeLib();
};
