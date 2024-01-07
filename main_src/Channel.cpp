#include "Channel.hpp"

Channel::Channel(const std::string &channelName, Client *client) : channelName(channelName), channelOperator(client)
{
	clientList.push_back(client);
}

const std::string 			&Channel::getChannelName() { return (channelName); }
const std::string 			&Channel::getChannelTopic(){ return (channelTopic); }
const std::string 			&Channel::getChannelMode() { return (channelMode); }
const std::vector<Client *> &Channel::getClientList() { return (clientList); }
const Client				*Channel::getChannelOperator() { return (channelOperator); }

void						Channel::setChannelTopic(const std::string &channelTopic) 
{
	this->channelTopic = channelTopic;
}

void						Channel::setChannelMode(const std::string &channelMode)
{
	this->channelMode = channelMode;
}

void						Channel::addClient(Client *client)
{
	clientList.push_back(client);
}

void						Channel::removeClient(Client *client)
{
	std::vector<Client *>::iterator iter = std::find(clientList.begin(), clientList.end(), client);
	if (iter != clientList.end())
	{
		clientList.erase(iter);
	}
}