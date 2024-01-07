#include <string>
#include <iostream>
#include <sstream>
#include <vector>
//class Message
//{
//	private:
//		std::string	msg;
//		std::string	command;
//		Message();

//	public:
//		std::string		commandNotifier();
//		std::string		messageNotifier();
//		Message(std::string command, std::string message);
//		~Message();
//};


class Message
{
	private:
		std::string								original;
		std::string								originalLeft;
		// PASS pass\r\n
		std::vector<std::string>				command;
		std::vector<std::vector<std::string> >	parameter;
		std::vector<std::string>				trailing;
	public:
		Message();
		void											parsing();
		const std::string								&getOriginal();
		const std::vector<std::string>					&getCommand();
		const std::vector<std::vector<std::string> >	&getParameter();
		const std::vector<std::string>	 				&getTrailing();
		void											addOriginal(std::string add);
};