/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayylaaba <ayylaaba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 10:03:40 by ayylaaba          #+#    #+#             */
/*   Updated: 2024/02/21 13:10:48 by ayylaaba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "request.hpp"
#include <sys/select.h>
#include "fd_info.hpp"
#include "multplixing.hpp"

int flag = 0;
size_t MAX_CLIENTS = 10;
int MAX_EVENTS = 1024;
int BUFFER_SIZE = 1024;
std::map<int, fd_info>  fd_maps; 

int main(int ac, char **av) 
{
    server              parse;
    int                 respo;
    request             rq;
    multplixing         mlp;

    if (ac < 2)
        parse.print_err("Argement Not Valid");
    
    parse.mange_file(av[1]);
    size_t i = 0;
    while (i < parse.s.size()) 
    {
        std::cout << "\n\nserver ----------------------------\n";
        std::map<std::string, std::string>::iterator  it;
        std::map<std::string, std::string>::iterator ite = parse.s[i]->cont.end();
        for (it = parse.s[i]->cont.begin(); it != ite; it++) 
        {
            if (!(*it).first.compare("listen"))
                std::cout << (*it).first << " " << (*it).second << "\n";
            else if (!(*it).first.compare("server_name"))
                std::cout << (*it).first << " " << (*it).second << "\n";
            else if ((*it).first == "root")
                std::cout << (*it).first << " " << (*it).second << "\n";
            else if ((*it).first == "error_page")
                std::cout << (*it).first << " " << (*it).second << "\n";
            else if ((*it).first == "host")
                std::cout << (*it).first << " " << (*it).second << "\n";
            else if ((*it).first == "client_max_body_size")
                std::cout << (*it).first << " " << (*it).second << "\n";
            else if ((*it).first == "index")
                std::cout << (*it).first << " " << (*it).second << "\n";
        }
        // std::cout << "\n\n LOCATION_size =  " << parse.s[i]->l.size() << "\n";
        // size_t j = 0;
        // while (j < parse.s[i]->l.size())
        // {
        //     std::cout << "\n\nlocation ----------------------------\n";
        //     std::cout << "=====> " << parse.s[i]->l[i]->allowed_methods[0] << " <====== \n";
        //     std::cout << "=====> " << parse.s[i]->l[i]->allowed_methods[1] << " <====== \n";
        //     std::map<std::string, std::string>::iterator  it_b = parse.l[j]->cont_l.begin();
        //     std::map<std::string, std::string>::iterator it_e = parse.l[j]->cont_l.end();
            
        //     while (it_b != it_e) 
        //     {
        //         if ((*it_b).first == "location")
        //             std::cout << "hooooooo" << (*it_b).first << " " << (*it_b).second << "\n";
        //         // if ((*it_b).first == "root")
        //         //     std::cout << (*it_b).first << " " << (*it_b).second << "\n";
        //         // if ((*it_b).first == "index")    
        //         //     std::cout << (*it_b).first << " " << (*it_b).second << "\n";
        //         // if ((*it_b).first == "limit_except")
        //         //     std::cout << (*it_b).first << " " << (*it_b).second << "\n";
        //         // if ((*it_b).first == "allow_methods")
        //         //     std::cout << (*it_b).first << " " << (*it_b).second << "\n";
        //         // if ((*it_b).first == "autoindex")
        //         //     std::cout << (*it_b).first << " " << (*it_b).second << "\n";
        //         // if ((*it_b).first == "upload")
        //         //     std::cout << (*it_b).first << " " << (*it_b).second << "\n";
        //         it_b++;
        //     }
        //     j++;
        // }
        i++;
    }
    mlp.lanch_server();

    std::string tmp;
    int bytesRead;

    while (1) 
    {
        std::cout << "\t\tI am listening for an event...\n";

        std::string buffer;
        int         client_sock;
        struct      epoll_event events[MAX_EVENTS];
        int NumFd = epoll_wait(mlp.epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < NumFd; i++) 
        {
            if (events[i].data.fd == mlp.serverSocket) 
            {
                std::cout << "connect a new client ...\n";
                client_sock = accept(mlp.serverSocket, NULL, NULL);

                struct epoll_event envts_client;
                envts_client.data.fd = client_sock;
                envts_client.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLRDHUP;
                if (epoll_ctl(mlp.epoll_fd, EPOLL_CTL_ADD, client_sock, &envts_client) == -1) 
                {
                    close(client_sock);
                    perror("Issue In Adding Client To Epoll");
                    exit(1);
                }
                parse.req_time[client_sock] = 0;
                fd_maps[events[i].data.fd]  = fd_info(); // each client has an obj of fd_info() that include methods an everything
            }
            else
            {
                std::map<int, fd_info>::iterator it_fd = fd_maps.find(events[i].data.fd);
                std::cout<< "\t\t Event was in this  Client ::: " <<  events[i].data.fd  << std::endl;
                if (events[i].events & EPOLLRDHUP /*&& !it->second*/)
                {
                    std::cout << " Enter epoll hrdhuo \n";
                    epoll_ctl(mlp.epoll_fd, EPOLL_CTL_DEL, events[i].data.fd , NULL);
                    close(events[i].data.fd  );
                    continue ;
                }
                if (events[i].events & EPOLLIN)
                {
                    std::cout << "FD READY TO READ -_- = " << events[i].data.fd << " \n";
                    buffer.resize(BUFFER_SIZE);
                    bytesRead = recv(events[i].data.fd , &buffer[0], BUFFER_SIZE, 0);
                    std::cout << "\n\n\t -> bytesRead ==== " << bytesRead << std::endl;
                    if (bytesRead > 0)
                        buffer.resize(bytesRead);
                    if (bytesRead <= 0)
                    {
                        epoll_ctl(mlp.epoll_fd, EPOLL_CTL_DEL, events[i].data.fd , NULL);
                        close(events[i].data.fd);
                        continue ;
                    }
                    if (flag == 0)
                    {
                        if (buffer.find("\r\n\r\n") != std::string::npos)
                        {
                            rq.parse_req(buffer, parse);
                            flag = 1;
                        }
                    }
                    if (rq.method == "POST" && flag == 1)
                    {
                        fd_maps[events[i].data.fd].post_.j = 0;
                        std::cout << "ENTER\n";
                        if (fd_maps[events[i].data.fd].post_.post_method(buffer, rq))
                            fd_maps[events[i].data.fd].post_.j = 1;
                    }
                    fd_maps[events[i].data.fd].requst       = rq; /* // must change.*/
                    fd_maps[events[i].data.fd].u_can_send   = 1; // must change.
                    fd_maps[events[i].data.fd].serv_        = parse; // must change.
                }
                else if (events[i].events & EPOLLOUT && !it_fd->second.rd_done && it_fd->second.u_can_send) // must not always enter to here i think ask about it 
                {
                    flag = 0;
                    std::cout << "ready  writing " << " \n";
                    respo = 0;
                    if (!fd_maps[events[i].data.fd].requst.method.compare("GET"))
                        respo = (*it_fd).second.get.get_mthod(events[i].data.fd);
                    if (!fd_maps[events[i].data.fd].requst.method.compare("POST") && fd_maps[events[i].data.fd].post_.j)
                    {
                        std::string response = "HTTP/1.1 201 OK\r\nContent-Type: text/html\r\n\r\nhello";
                        send(events[i].data.fd,response.c_str(), response.length(), 0);
                        respo = 1;
                    }
                    std::cout << "\t\t stat kaml wla ba9i == " << it_fd->second.rd_done << std::endl;
                    if (respo)
                    {
                        std::cout << "\t\t SF KAML GHADI UTM7A HAD "  << events[i].data.fd << std::endl;
                        epoll_ctl(mlp.epoll_fd, EPOLL_CTL_DEL, events[i].data.fd , NULL);
                        close(events[i].data.fd);
                        fd_maps.erase(events[i].data.fd);
                        continue ;
                    }
                }
            }
        }
    }
    return 0;
}
