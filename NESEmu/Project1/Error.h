#pragma once
#include <queue>
#include <string>

class Error {
	Error();
	~Error();
	static Error*			instance;
	std::queue<std::string>	errors;
public:
	static Error*	getInstance();
	void			queue(std::string);
};