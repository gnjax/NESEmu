#pragma once

#ifdef	_WIN32
# include "LoadLibWindows.h"
#else
# include "LoadLibLinux.h"
#endif

template<class T =
#ifdef _WIN32
	LoadLibWindows
#else
	LoadLibLinux
#endif
>

class LoadLib
{
private:
	T		_load; 
public:
	LoadLib() {}
	~LoadLib() {}

	void	loadLibrary(char const *name) {
		this->_load.loadLibrary(name);
	}

	void*	getLib(char const *name) {
		return (this->_load.getLib(name));
	}

	bool	closeLib() {
		return (this->_load.closeLib());
	}

	template<typename U>
	U*		getInstance() {
		U*	*(*ptr)();
		return reinterpret_cast<U *(*)()>(this->getLib("getInstance"))();
	}
};