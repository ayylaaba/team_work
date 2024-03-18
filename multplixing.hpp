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


class multplixing
{
    public:
        void    lanch_server();
        int     epoll_fd;
        int                 serverSocket;

        multplixing(){

        };
};

#endif
