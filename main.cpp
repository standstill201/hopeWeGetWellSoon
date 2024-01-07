#include "./main_src/Server.hpp"

int main(int ac, char *av[])
{ // 인자 포트, 패스워드 받아야함
	if (ac != 2)
	{
		std::cout << "arguments error" << std::endl;
		return (1);
	}
	Server	server(atoi(av[1]));
	server.runServer();
}