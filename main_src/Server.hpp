#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/event.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <sstream>

#include "../sub_src/Tools.hpp"
#include "./Client.hpp"
#include "./Message.hpp"

class Client;
class Channel;

class Server
{
	private:
		// 2 버버퍼 여기에 만들기
		struct kevent						eventList[8];
		struct kevent						*currEvent;
		int									serverSocket;
		int									kq;
		std::vector<struct kevent>			changeList;
		std::map<int, Client *>				clientMap;
		std::map<std::string, Channel *>	channelMap;
		char								buf[1024];
		// 512글자 초과할때 생각해야함
		std::map<int, Message>				messageMap;
		std::map<int, std::string>			writeBuffers;

		std::string serverCreationDate;
		
		void writeEventSend(int client_fd, const std::string& message);
		void eventError();
		void eventRead();
		void eventWrite();
		void serverSocketEvent();
		void clientSocketEvent();
		void changeEvents(std::vector<struct kevent>& changeList, uintptr_t ident, int16_t filter,
					uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void disconnect_client(int client_fd, std::map<int, Client *>& clientMap);
		void processUserAuthentication(Message msg);
	public:
		Server(int port);
		void									runServer();
		const std::string 						&getSeverCreationDate();
};

#endif