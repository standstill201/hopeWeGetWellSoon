#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/event.h>
#include <map>
#include <vector>
#include <unistd.h>

void changeEvents(std::vector<struct kevent>& changeList, uintptr_t ident, int16_t filter,
					uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
	changeList.push_back(tempEvent);
}

void disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
	std::cout << "client disconnected: " << client_fd <<  std::endl;
	close(client_fd);
	clients.erase(client_fd);
}

int main(int ac, char *av[])
{
	if (ac != 2)
	{
		std::cout << "arguments error" << std::endl;
		return (1);
	}

	int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == -1)
	{
		std::cout << "socket creation failed" << std::endl;
	}
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(av[1]));
	if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cout << "bind failed" << std::endl;
	}
	if (listen(serverSocket, 5) == -1)
	{
		std::cout << "listen failed" << std::endl;
	}
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "fcntl failed" << std::endl;
	}

	int kq = kqueue();
	if (kq == -1)
	{
		std::cout << "kqueue failed" << std::endl;
	}
	std::map<int, std::string> clients;
	std::vector<struct kevent> changeList;
	struct kevent eventList[8];

	changeEvents(changeList, serverSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "echo server started" << std::endl;

	int newEvents;
	struct kevent *currEvent;
	while (true)
	{
		newEvents = kevent(kq, &changeList[0], changeList.size(), eventList, 8, NULL);
		if (newEvents == -1)
		{
			std::cout << "kevent failed" << std::endl;
		}
		changeList.clear();

		for (int i = 0; i < newEvents; ++i)
		{
			currEvent = &eventList[i];

			if (currEvent->flags & EV_ERROR)
			{
				if (currEvent->ident == serverSocket)
				{
					std::cout << "server socket error" << std::endl;
				}
				else
				{
					std::cout << "client socket error" << std::endl;
					disconnect_client(currEvent->ident, clients);
				}
			}
			else if (currEvent->filter == EVFILT_READ)
			{
				if (currEvent->ident == serverSocket)
				{
					int clientSocket = accept(serverSocket, NULL, NULL);
					if (clientSocket == -1)
					{
						std::cout << "accept failed" << std::endl;
					}
					std::cout << "accept new client: " << clientSocket << std::endl;
					fcntl(clientSocket, F_SETFL, O_NONBLOCK);
					changeEvents(changeList, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					changeEvents(changeList, clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					clients[clientSocket] = "";
				}
				else if (clients.find(currEvent->ident) != clients.end())
				{
					char buf[1024];
					int n = read(currEvent->ident, buf, sizeof(buf));

					if (n <= 0)
					{
						if (n < 0)
						{
							std::cout << "client read error" << std::endl;
						}
						disconnect_client(currEvent->ident, clients);
					}
					else
					{
						buf[n] = '\0';
						clients[currEvent->ident] += buf;
						std::cout << "received data from " << currEvent->ident << ": " << clients[currEvent->ident] << std::endl;
					}
				}
			}
			else if (currEvent->filter == EVFILT_WRITE)
			{
				std::map<int, std::string>::iterator iter = clients.find(currEvent->ident);
				if (iter != clients.end())
				{
					if (clients[currEvent->ident] != "")
					{
						int n = write(currEvent->ident, clients[currEvent->ident].c_str(), clients[currEvent->ident].size());
						if (n == -1)
						{
							std::cout << "client write error" << std::endl;
							disconnect_client(currEvent->ident, clients);
						}
						else
						{
							clients[currEvent->ident].clear();
						}
					}
				}
			}
		}
	}
	return (0);
}


		// std::cout << "Command:" << std::endl;
		// const auto& commands = msg.getCommand();
		// for (int i = 0; i < commands.size(); ++i) {
		// 	std::cout << "Command[" << i << "]: " << commands[i] << std::endl;
		// }

		// // parameters 출력
		// std::cout << "Parameters:" << std::endl;
		// const auto& params = msg.getParameter();
		// for (int i = 0; i < params.size(); ++i) {
		// 	std::cout << "Parameter[" << i << "]: ";
		// 	for (int j = 0; j < params[i].size(); ++j) {
		// 		std::cout << params[i][j] << " ";
		// 	}
		// 	std::cout << std::endl;
		// }

		// // trailing 출력
		// std::cout << "Trailing:" << std::endl;
		// const auto& trailing = msg.getTrailing();
		// for (int i = 0; i < trailing.size(); ++i) {
		// 	std::cout << "Trailing[" << i << "]: " << trailing[i] << std::endl;
		// }
