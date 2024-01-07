#include "./Client.hpp"


Client::Client(int clientSocket)
{
	this->clientSocket = clientSocket;
	this->authPass = 0;
	this->authNick = 0;
	this->authUser= 0;
	is_login = 0;
	is_check = 0;
}

Client::Client()
{
	
}

Client::~Client()
{
}

const std::map<std::string, Channel *> &Client::getChannelMap() { return (channelMap); }