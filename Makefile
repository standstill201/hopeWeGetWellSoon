NAME = ircserv

CC = c++

CFLAGS = -std=c++98 #-Wall -Wextra -Werror

SRC =	./main.cpp \
		./sub_src/Tools.cpp \
		./sub_src/Error.cpp \
		./main_src/Server.cpp \
		./main_src/Message.cpp \
		./main_src/Client.cpp \
		./main_src/Channel.cpp \

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ)
		$(CC) $(OBJ) -o $(NAME)

%.o : %.cpp
		$(CC) $(CFLAGS) -c $^ -o $@

clean :
	rm -rf $(OBJ)

fclean :
	rm -rf $(OBJ) $(NAME)

re : fclean
	make -j8

.PHONY: all clean fclean re
