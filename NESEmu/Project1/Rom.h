#pragma once
#include <cstdlib>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Error.h"

#define		PGR_SIZE	0x4000
#define		CHR_SIZE	0x2000
#define		PGR_OFFSET	0xF

class Rom {
	const char	magic[4];
	char		pgrPage;
	char		chrPage;
	char		mapper;
	bool		mirroring;
	char*		rom;
	boost::uintmax_t	size;
public:
	Rom();
	~Rom();
	bool		checkFile(std::string & s);
	bool		loadFile(std::string & s);
	bool		checkHeaders();
	bool		initialize(std::string);
	void loadIntoMemory(char * memory);
};

