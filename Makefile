NAME = webserve

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

SRCS = 	main.cpp \
		server.cpp \
		location.cpp \
		request.cpp \
		fd_info.cpp \
		get_method.cpp \
		./post/for_body.cpp \
		./post/for_header.cpp \
		./post/get_extension.cpp \
		./post/helpers.cpp \
		./post/post_methods.cpp \

OBJ = ${SRCS:.cpp=.o}

all : ${NAME}

${NAME} : ${OBJ}
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean :
	rm -f ${OBJ} outfile*

fclean : clean
	rm -f ${NAME}

re : fclean all