#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <map>

#include "./Channel.hpp"

class Channel;

class Client
{
	private:
		int									clientSocket;
		std::map<std::string, Channel *>	channelMap;
	public:
	//수정필요
		Client(/* args */);
		int								is_check;
		int								is_login;
		std::string						nickName;
		std::string						userName;
		int								authPass; 
		int								authNick;
		int								authUser;
		const std::map<std::string, Channel *> &getChannelMap();
		Client(int clientSocket);
		~Client();
};

#endif
