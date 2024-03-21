#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include <sys/epoll.h>
#include "get_method.hpp"
#include "post.hpp"

class Client
{
    public:
        int                 res_header;
        std::string         stor_uri;
        std::ifstream       read_f;
        int                 rd_done;
        int                 res_sent;
        int                 u_can_send;
        std::streampos      filePosition;
        get_method          get;
        request             requst;
        post                post_;
        
        Client(std::string uri_);
        Client(const Client& copy);
        Client&         operator=(const Client& copy);
        Client&         operator=(std::string _uri);
        Client();
        ~Client();
};

#endif
