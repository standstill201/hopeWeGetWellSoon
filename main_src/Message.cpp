#include "Message.hpp"


Message::Message(){}

void	Message::addOriginal(std::string add)
{
	size_t pos;

	originalLeft += add;
	command.clear();
	parameter.clear();
	trailing.clear();
	std::cout << "original: " << original << std::endl;
	// 개행 문자('\n') 또는 문자열의 끝('\0')을 찾을 때까지 반복
	while ((pos = originalLeft.find_first_of("\n\0")) != std::string::npos)
	{
		std::string toAdd = originalLeft.substr(0, pos);
		original += toAdd;
		originalLeft.erase(0, pos + 1);
	}
	std::cout << "original: " << original << std::endl;
	
	// orleft 가 남아있는 경우
		//orleft에 구분문자가 있는 경우
			//original에 구분문자 전까지 orleft더해주고 orleft의 구분문자 이전까지 삭제.
		//orleft에 구분문자가 없는 경우
			//orleft를 original에 더함. orleft 전체 삭제
}


void Message::parsing()
{
	std::istringstream			stream(original);
	std::string					line;
	std::string					tmpCommand;
	std::string					tmpTrailing;

	std::string					param;
	while (std::getline(stream, line, '\r'))
	{
		std::vector<std::string>	tmpParameter;
		if (stream.peek() == '\n')
			stream.get();
		std::istringstream	iss(line);
		iss >> tmpCommand;
		command.push_back(tmpCommand);
		while (iss >> param)
		{
			if (param[0] == ':')
			{
				std::string tmp = "";
				std::getline(iss, tmpTrailing);
				tmp += param.substr(1);
				tmp += tmpTrailing;
				tmpTrailing = tmp;
				trailing.push_back(tmpTrailing);
				break ;
			}
			tmpParameter.push_back(param);
		}
		parameter.push_back(tmpParameter);
	}
	if (command.size() != 0)
		original.clear();
}

const std::string &Message::getOriginal() { return (original); }
const std::vector<std::string> &Message::getCommand() { return (command); }
const std::vector<std::vector<std::string> >&Message::getParameter(){ return (parameter); }
const std::vector<std::string> &Message::getTrailing() { return (trailing); }


/*
int main()
{
	std::string str = "PRIVMSG #[channelname] :[message]";

	Message msg(str);

	msg.parsing();

	std::cout << "Original: " << msg.getOriginal() << std::endl;
	std::cout << "Command: " << msg.getCommand() << std::endl;
	std::cout << "Parameters: ";
	std::vector<std::string> param = msg.getParameter();
	for (std::vector<std::string>::iterator iter = param.begin(); iter != param.end(); ++iter)
	{
		std::cout << *iter << " ";
	}
	std::cout << std::endl;
	std::cout << "Trailing: " << msg.getTrailing() << std::endl;
}
*/

/*
/nick [nickname] : 닉네임 변경
NICK [nickname]

/set user_name [username] : 사용자 이름 설정
USER [username] 0 * :[realname]

/join #[channelname] : 채널 참여
JOIN #[channelname]

/part #[channelname] [optional message] : 채널 나가기
PART #[channelname] [optional message]

/list : 채널 목록 보기
LIST

/msg [nickname] [message] : 다른 사용자에게 메시지 보내기
PRIVMSG [nickname] :[message]

채널에 들어가서 채팅 칠 때
PRIVMSG #[channelname] :[message]

/kick #[channelname] [username] [optional reason] : 채널에서 사용자 강제 퇴장
KICK #[channelname] [username] :[optional reason]
KICK #:sadf:: :username::: :optional::
/invite [nickname] #[channelname] : 사용자를 채널로 초대
INVITE [nickname] :#[channelname]

/topic #[channelname] [new topic] : 채널 주제 변경
TOPIC #[channelname] :[new topic]

/mode #[channelname] [+/-][mode] : 채널 모드 변경
MODE #[channelname] [+/-][mode]
*/