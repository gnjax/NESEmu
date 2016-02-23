#include "Error.h"

Error*	Error::instance = nullptr;

Error::Error() {

}

Error::~Error() {

}

Error* Error::getInstance()
{
	if (instance == nullptr)
		instance = new Error();
		return instance;
}

void Error::queue(std::string s)
{
	this->errors.push(s);
}

void	Error::display() {
	while (!this->errors.empty()) {
		std::cerr << this->errors.front() << std::endl;
		this->errors.pop();
	}
}