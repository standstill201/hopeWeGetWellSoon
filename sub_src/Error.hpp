#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>

class Error
{
	private:
		Error(/* args */);
		~Error();
	public:
		static	void	fatalError(std::string errmsg);
};



#endif