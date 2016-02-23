#pragma once
#include <queue>
#include <string>
#include <iostream>

class Error {
	Error();
	~Error();
	static Error*			instance;
	std::queue<std::string>	errors;
public:
	static Error*	getInstance();
	void			queue(std::string);
	void display();
};