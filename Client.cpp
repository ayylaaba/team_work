#include "Client.hpp"
#include "request.hpp"
#include "get_method.hpp"

Client::Client(std::string uri_)
{
    stor_uri    = uri_;
    res_header  = 0;
    rd_done     = 0;
    res_sent    = 0;
    u_can_send  = 0; 
}


Client::Client(const Client& copy)
{
    this->u_can_send  = copy.u_can_send; 
    this->filePosition = copy.filePosition;
    this->rd_done = copy.rd_done;
    this->res_header = copy.res_header;
    this->stor_uri = copy.stor_uri;
}

// Client&         Client::operator=(std::string _uri)
// {
//     stor_uri = _uri;
//     return (*this);
// }

Client&         Client::operator=(const Client& copy)
{
    this->u_can_send  = copy.u_can_send; 
     this->filePosition = copy.filePosition;
     this->rd_done      = copy.rd_done;
    //  this->read_f    = copy.read_f;
     this->res_header   = copy.res_header;
     this->stor_uri     = copy.stor_uri;
     this->res_sent     = copy.res_sent;
    return (*this);
}

Client::~Client()
{
}

Client::Client()
{
    res_header  = 0;
    rd_done     = 0;
    u_can_send  = 0;
    res_sent    = 0;
}

// {
//     first_time;
//     {
//         check request requerments
//     }
//     file_requested;
// }

// get_method
// {
//     first_time;
//     file_read;
//     end_of_send;
// }

