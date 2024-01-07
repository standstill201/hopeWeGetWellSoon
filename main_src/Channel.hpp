#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

class Client;

class Channel
{
	private:
		std::string				channelName;
		std::string				channelTopic;
		std::string				channelMode;
		std::vector<Client *>	clientList;
		Client 					*channelOperator;
	public:
		Channel(const std::string &channelName, Client *client);
		const std::string 			&getChannelName();
		const std::string 			&getChannelTopic();
		const std::string 			&getChannelMode();
		const std::vector<Client *> &getClientList();
		const Client				*getChannelOperator();
		void						setChannelTopic(const std::string &channelTopic);
		void						setChannelMode(const std::string &channelMode);
		void						addClient(Client *client);
		void						removeClient(Client *client);
};
#endif