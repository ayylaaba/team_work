#ifndef MULTIPLIXING_HPP
#define MULTIPLIXING_HPP



#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include "Client.hpp"

# define MAX_EVENTS 1024
# define BUFFER_SIZE 1024

class multplixing
{
    public:
        void    lanch_server(server parse);server              parse;
        int     epoll_fd;
        int     serverSocket;

        multplixing(){

        };
};

#endif
