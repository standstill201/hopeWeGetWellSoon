#include "./Tools.hpp"

bool Tools::isValidNickname(const std::string& nickname)
{
	if (nickname.empty())
	{
		return false; // Nicknames must be non-empty
	}
	// Characters not allowed in a nickname
	const std::string invalidChars = " ,*?!@";

	// Characters not allowed at the start of a nickname
	const std::string invalidStartChars = "$:";

	// Check for invalid starting characters
	if (invalidStartChars.find(nickname[0]) != std::string::npos)
	{
		return false;
	}

	// Check each character of the nickname
	for (std::string::const_iterator ch = nickname.cbegin(); ch != nickname.cend(); ++ch)
	{
		// Check for any invalid characters
		if (invalidChars.find(*ch) != std::string::npos)
		{
			return false;
		}

		// Check for dot character ('.')
		if (*ch == '.')
		{
			return false; // Should not contain, but it's not a strict rule
		}
	}

	return true;
}


bool Tools::isValidChannelName(const std::string& channelName)
{
	// Check if the channel name is empty or does not start with a valid prefix
	if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&')) // Add other valid prefixes as needed
		return false;

	// Check for invalid characters
	if (channelName.find_first_of(" ,\x07") != std::string::npos)
		return false;

	return true;
}
