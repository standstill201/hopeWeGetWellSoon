#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>

class Tools
{
private:
	/* data */
public:
	Tools(/* args */);
	~Tools();
	static bool isValidNickname(const std::string& nickname);
	static bool isValidChannelName(const std::string& channelName);

};

#endif