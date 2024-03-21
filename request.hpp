#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <fstream>
#include <string>


#include <iostream>
#include <string>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include "server.hpp"
#include <sys/epoll.h>


class server;
class request
{
    public:
        std::string content_type;
        std::string content_length;
        std::string transfer_encoding;
        std::string uri;
        std::string path;
        std::string method;
        std::string http_version;
        std::map<std::string, std::string> extentions;    

        bool        auto_index_stat;
        unsigned long g;
        size_t check;

        /*************** REQUEST FUNCTIONS ***************/

        void            parse_header(std::string buffer, server &serv);
        std::string     get_delet_resp(std::string path, int stat);
        void            fill_extentions();
        std::streampos  get_fileLenth(std::string path);
        std::string     get_exten_type(std::string path);
        bool            check_autoindex(std::map<std::string, std::string> loca_map);
        std::string     get_full_uri(std::string path, server &server);
        std::string     delet_method(std::string path, server &server);
        void            parse_req(std::string   rq, server &server);
  
        template <typename T>
        std::string to_string(T value) 
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
        request();
        ~request();
};


#endif