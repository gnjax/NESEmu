#include "Rom.h"

Rom::Rom() : magic{0x4e,0x45,0x53,0x1a} {
	this->rom = nullptr;
}

Rom::~Rom() {
	if (this->rom != nullptr)
		delete this->rom;
}

/*
** PARAMETERS :
**	string s :	Path to the Rom
** FUNCTION :
**	Check file existence and size
** RETURN :
**	TRUE :		The path leads to a valid file
**	FALSE :		Path is incorrect
*/
bool	Rom::checkFile(std::string &s) {
	boost::filesystem::path			path{ s };
	boost::filesystem::file_status	status;
	boost::system::error_code		ec;

	//Check if path is correct
	try {
		status = boost::filesystem::status(path);
	}
	catch (boost::filesystem::filesystem_error &e) {
		Error::getInstance()->queue(e.what());
		return (false);
	}

	//Check if the provided path is a regular file
	if (!boost::filesystem::is_regular_file(status)) {
		Error::getInstance()->queue("The provided path is not a regular file");
		return (false);
	}

	//Get filesize
	this->size = boost::filesystem::file_size(path, ec);
	if (ec) {
		Error::getInstance()->queue(ec.message());
		return (false);
	}
	return (true);
}

/*
** PARAMETERS :
**	string s :	Path to the Rom
** FUNCTION :
**	Copy rom file content to rom array
** RETURN :
**	TRUE :		The Rom was correctly copied to the array
**	FALSE :		Something failed in file reading
*/
bool	Rom::loadFile(std::string &s) {
	std::ifstream	fin(s, std::ifstream::binary);

	if (!fin.is_open()) {
		Error::getInstance()->queue("Could not open file");
		return (false);
	}
	this->rom = new char[this->size]();
	fin.read(this->rom, this->size);
	if (!fin) {
		Error::getInstance()->queue("Could only read less charachters than expected from file");
		return (false);
	}
	fin.close();
	return (true);
}

/*
** PARAMETERS :
**	None
** FUNCTION :
**	Check rom headers to make sure it's in correct iNes format
** RETURN :
**	TRUE :		Headers succesfully identified
**	FALSE :		Wrong file format
*/
bool	Rom::checkHeaders() {
	//Check iNes magic number, equal to "NES->"
	for (int i = 0; i < 4; ++i) {
		if (this->magic[i] != this->rom[i]) {
			Error::getInstance()->queue("Could not validate iNes header");
			return (false);
		}
	}
	//Get PGR-ROM (program code) pages number
	this->pgrPage = this->rom[4];
	//Get CHR-ROM (tiles bank) pages number
	this->chrPage = this->rom[5];
	//Get Mapper number -> MSB and mirroring (horizontal/vertical) -> LSB.
	char	tmp = this->rom[6];
	this->mapper = (tmp >> 4);
	this->mirroring = (bool)(tmp & 0b00001111);
	return (true);
}

/*
** PARAMETERS :
**	string s :	Path to the Rom
** FUNCTION :
**	Load and parse the given rom
** RETURN :
**	TRUE :		Rom succesfully loaded
**	FALSE :		Error in Rom loading
*/
bool	Rom::initialize(std::string s) {
	if (!this->checkFile(s))
		return (false);
	if (!this->loadFile(s))
		return (false);
	if (!this->checkHeaders())
		return (false);
	return (true);
}


/*
** PARAMETERS :
**	char* memory :	CPU Memory
** FUNCTION :
**	Load the PGR pages into CPU Memory
** RETURN :
**	None
*/
void	Rom::loadIntoMemory(char *memory) {
	memcpy(memory + 0x8000, this->rom + PGR_OFFSET, PGR_SIZE);
	memcpy(memory + 0xC000, this->rom + PGR_OFFSET + ((this->chrPage > 1) ? (PGR_SIZE) : (0)), PGR_SIZE);
}