#include "./Server.hpp"

Server::Server(int port)
{
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == -1)
	{
		std::cout << "socket creation failed" << std::endl;
	}
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
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
	// Get the current date and time
	std::time_t now = std::time(nullptr);
	struct tm* currentTime = std::localtime(&now);

	// Format the current date as a string in the desired format
	char dateStr[20];
	std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", currentTime);

	// Assign the formatted date to serverCreationDate
	serverCreationDate = dateStr;
}

void Server::runServer()
{
	kq = kqueue();
	if (kq == -1)
	{
		std::cout << "kqueue failed" << std::endl;
	}

	this->changeEvents(changeList, serverSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "echo server started" << std::endl;

	int newEvents;
	while (true)
	{
		newEvents = kevent(kq, &changeList[0], changeList.size(), eventList, 8, NULL);
		if (newEvents == -1)
		{
			std::cout << "kevent failed" << std::endl;
			exit(1);
		}
		changeList.clear();

		for (int i = 0; i < newEvents; ++i)
		{
			currEvent = &eventList[i];

			if (currEvent->flags & EV_ERROR)
			{
				this->eventError();
			}
			else if (currEvent->filter == EVFILT_READ)
			{
				this->eventRead();
			}
			else if (currEvent->filter == EVFILT_WRITE)
			{
				this->eventWrite();
			}
		}
	}
}



void Server::writeEventSend(int client_fd, const std::string& message)
{
	// Store the message in the write buffer
	writeBuffers[client_fd] = message;

	// Set a write event for this client
	this->changeEvents(changeList, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, NULL);
}


void Server::eventError()
{
	if (currEvent->ident == static_cast<uintptr_t>(serverSocket))
	{
		std::cout << "server socket error" << std::endl;
	}
	else
	{
		std::cout << "client socket error" << std::endl;
		this->disconnect_client(currEvent->ident, clientMap);
	}
}

void Server::serverSocketEvent()
{
	int clientSocket = accept(serverSocket, NULL, NULL);
	if (clientSocket == -1)
	{
		std::cout << "accept failed" << std::endl;
	}
	std::cout << "accept new client: " << clientSocket << std::endl;
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	this->changeEvents(changeList, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	clientMap.insert(std::make_pair(clientSocket, new Client(clientSocket)));
	messageMap.insert(std::make_pair(clientSocket, Message()));
}

void Server::processUserAuthentication(Message msg)
{
	for (int i = 0; i < msg.getCommand().size(); ++i)
	{
		if (msg.getCommand()[i] == "PASS" && msg.getParameter()[i][0] == "pass")
			clientMap[currEvent->ident]->authPass = 1;
		else if (msg.getCommand()[i] == "NICK")
		{
			clientMap[currEvent->ident]->nickName = msg.getParameter()[i][0];
			clientMap[currEvent->ident]->authNick = 1;
		}
		else if (msg.getCommand()[i] == "USER")
		{
			clientMap[currEvent->ident]->userName = msg.getParameter()[i][0];
			clientMap[currEvent->ident]->authUser = 1;
		}
	}
	if (clientMap[currEvent->ident]->authNick == 1 && clientMap[currEvent->ident]->authPass == 1 && clientMap[currEvent->ident]->authUser == 1 && clientMap[currEvent->ident]->is_login == 0)
	{
		clientMap[currEvent->ident]->is_login = 1;
		std::string welcomeMessage;

		welcomeMessage += ":irc.HopeWeGetWellSoon.com 001 " + clientMap[currEvent->ident]->nickName + " :Welcome to the IRC Network, " + clientMap[currEvent->ident]->nickName + "!\r\n";
		welcomeMessage += ":irc.HopeWeGetWellSoon.com 002 " + clientMap[currEvent->ident]->nickName + " :Your host is irc.HopeWeGetWellSoon.com, running version 1.0\r\n";
		welcomeMessage += ":irc.HopeWeGetWellSoon.com 003 " + clientMap[currEvent->ident]->nickName + " :This server was created " + serverCreationDate + "\r\n";
		welcomeMessage += ":irc.HopeWeGetWellSoon.com 004 " + clientMap[currEvent->ident]->nickName + " irc.HopeWeGetWellSoon.com 1.0 iw oitkl\r\n";

		writeEventSend(currEvent->ident, welcomeMessage);
	}
	else
	{
		// 여기에 뭐가 아직 안들어왔는지 알려주는 메세지 넣을것.

	}
}


void Server::clientSocketEvent()
{
	memset(buf, 0, 1024);
	int n = read(currEvent->ident, buf, sizeof(buf));

	buf[n] = '\0';
	if (n <= 0)
	{
		if (n < 0)
		{
			std::cout << "client read error" << std::endl;
		}
		this->disconnect_client(currEvent->ident, clientMap);
	}
	else
	{
		messageMap[currEvent->ident].addOriginal(buf);
		// 현재 해야 하는 것. 메세지에 들어오는 값이 pa\r\n EOFss pass \r\n USER user이런 식으로 들어오는 경우가 있음
		messageMap[currEvent->ident].parsing();

		std::cout << "------------------\n";
		std::cout << "Command:" << std::endl;
		const auto& commands = messageMap[currEvent->ident].getCommand();
		for (int i = 0; i < commands.size(); ++i) {
			std::cout << "Command[" << i << "]: " << commands[i] << std::endl;
		}

		// parameters 출력
		std::cout << "Parameters:" << std::endl;
		const auto& params = messageMap[currEvent->ident].getParameter();
		for (int i = 0; i < params.size(); ++i) {
			std::cout << "Parameter[" << i << "]: ";
			for (int j = 0; j < params[i].size(); ++j) {
				std::cout << params[i][j] << " ";
			}
			std::cout << std::endl;
		}

		// trailing 출력
		std::cout << "Trailing:" << std::endl;
		const auto& trailing = messageMap[currEvent->ident].getTrailing();
		for (int i = 0; i < trailing.size(); ++i) {
			std::cout << "Trailing[" << i << "]: " << trailing[i] << std::endl;
		}
		std::cout << "------------------\n";
		if (clientMap[currEvent->ident]->is_login == 0)
		{
			processUserAuthentication(messageMap[currEvent->ident]);
		}
		else
		{
			for (int i = 0; i < messageMap[currEvent->ident].getCommand().size(); ++i)
			{
				if (messageMap[currEvent->ident].getCommand()[i] == "NICK")
				{
					if (!Tools::isValidNickname(clientMap[currEvent->ident]->nickName))
					{
						std::cout << "Invalid nickname: " << clientMap[currEvent->ident]->nickName << std::endl;
						// Handle invalid nickname case (e.g., send error message to client)
						continue;
					}
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "JOIN")
				{
					std::string channelName = messageMap[currEvent->ident].getParameter()[i][0];

					// Validate channel name
					if (Tools::isValidChannelName(channelName))
					{
						// Check if channel exists, if not create it
						if (channelMap.find(channelName) == channelMap.end())
						{
							channelMap[channelName] = new Channel(channelName, clientMap[currEvent->ident]);
						}

						// Add this client to the channel
						channelMap[channelName]->addClient(clientMap[currEvent->ident]);

						// Send a confirmation message to the client
						std::string joinMessage = "Joined channel " + channelName + "\r\n";
						writeEventSend(currEvent->ident, joinMessage);
					}
					else
					{
						// Send an error message for invalid channel name
						std::string errorMessage = "Error: Invalid channel name '" + channelName + "'\r\n";
						writeEventSend(currEvent->ident, errorMessage);
					}
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "PART")
				{
					std::string channelName = messageMap[currEvent->ident].getParameter()[i][0];
					std::map<std::string, Channel *> clientChannelMap = clientMap[currEvent->ident]->getChannelMap();
					std::map<std::string, Channel *>::iterator iter = clientChannelMap.find(channelName);
					if (iter != clientChannelMap.end())
					{
						(iter->second)->removeClient(clientMap[currEvent->ident]);
						clientMap
					}
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "PRIVMSG")
				{
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "KICK")
				{
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "INVITE")
				{
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "TOPIC")
				{
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "MODE")
				{
				}
				else if (messageMap[currEvent->ident].getCommand()[i] == "PING")
				{
				}
				// quit exit fd 값 사라지면 나갔다고 알려줘야함.
			}
		}
	}
}

void Server::eventRead()
{
	if (currEvent->ident == static_cast<uintptr_t>(serverSocket))
	{
		this->serverSocketEvent();
	}
	else if (clientMap.find(currEvent->ident) != clientMap.end())
	{
		this->clientSocketEvent();
	}
}

void Server::eventWrite()
{
	int client_fd = currEvent->ident;

	// Check if the client has a message in the write buffer
	if (writeBuffers.find(client_fd) != writeBuffers.end())
	{
		// Send the message
		const std::string& message = writeBuffers[client_fd];
		send(client_fd, message.c_str(), message.size(), 0);

		// Clear the message from the buffer
		writeBuffers.erase(client_fd);
	}
}

void Server::changeEvents(std::vector<struct kevent>& changeList, uintptr_t ident, int16_t filter,
			uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent tempEvent;

	EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
	changeList.push_back(tempEvent);
}

void Server::disconnect_client(int client_fd, std::map<int, Client *>& clientMap)
{
	std::cout << "client disconnected: " << client_fd <<  std::endl;
	close(client_fd);
	clientMap.erase(client_fd);
}

const std::string &Server::getSeverCreationDate() { return (serverCreationDate); }
