#include "./Error.hpp"

Error::Error(/* args */)
{
}

Error::~Error()
{
}

void	Error::fatalError(std::string errmsg)
{
	throw std::runtime_error(errmsg);
}
