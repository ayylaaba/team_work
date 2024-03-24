#include "multplixing.hpp"

#define MAX_CLIENTS 

std::map<int, Client>  fd_maps; 
int flag = 0;


void        multplixing::lanch_server(server parse)
{
    int bytesRead;
    int respo;
    request     rq;

    // if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    //     perror("Socket creation failed");
    //     exit(EXIT_FAILURE);
    // }

    // // Initialize server address structure
    // sockaddr_in serverAddr;
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_port = htons(8080);
    // serverAddr.sin_addr.s_addr = INADDR_ANY;
    // int sp = 1;
    // // Bind the socket
    // setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &sp, sizeof(sp));
    // if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1) 
    // {
    //     perror("Bind failed");
    //     exit(EXIT_FAILURE);
    // }

    // // Listen for incoming connections
    // if (listen(serverSocket, 5) == -1) {
    //     perror("Listen failed");
    //     exit(EXIT_FAILURE);
    // }

    // std::cout << "Server is listening on port 8080...\n";

    // epoll_fd = epoll_create(5);
    // struct epoll_event envts;
    // envts.data.fd = serverSocket;
    // envts.events = EPOLLIN;

    // epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &envts);

    std::vector<server*>::iterator it;

    epoll_fd = epoll_create(5);

    for (it = parse.s.begin(); it != parse.s.end(); it++)
    {
        int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        serverSocket.push_back(sockfd);

        sockaddr_in sock_info;

        sock_info.sin_family = AF_INET;
        sock_info.sin_port = htons(string_to_int((*it)->cont["listen"]));
        sock_info.sin_addr.s_addr = INADDR_ANY;
        int sp = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &sp, sizeof(sp));
        if (bind(sockfd, (struct sockaddr *)&sock_info, sizeof(sock_info))) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(sockfd, 5)) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        int flag;
    
        if ((flag = fcntl(sockfd, F_GETFL, 0)) < 0) {
            perror("Failed getting file status");
            exit(EXIT_FAILURE);
        }
        flag |= O_NONBLOCK;
        if (fcntl(sockfd, F_SETFL, flag) < 0) {
            perror("Failed setting file status");
            exit(EXIT_FAILURE);
        }

        
        struct epoll_event envts;
        envts.data.fd = sockfd;
        envts.events = EPOLLIN;
    
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &envts);

        std::cout << "Server is listening on port '" << (*it)->cont["listen"] << "'...\n";
    }

    while (true) {
            std::string buffer;
            std::vector<int>::iterator it;
            int num = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

            for (int i = 0; i < num; i++) {
                if ((it = std::find(serverSocket.begin(), serverSocket.end(), events[i].data.fd)) != serverSocket.end()) {
                    std::cout << "New Client Connected\n";
                    int client_socket = accept(*it, NULL, NULL);

                    struct epoll_event envts_client;
                    envts_client.data.fd = client_socket;
                    envts_client.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLRDHUP;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &envts_client) == -1){
                        close(client_socket);
                        perror("Issue In Adding Client To Epoll");
                        exit(1);
                    }
                    parse.req_time[client_socket] = 0;
                    fd_maps[events[i].data.fd] = Client();
                    std::cout << "Client " << client_socket << " Added To Map\n";
                }
                else {
                    std::map<int, Client>::iterator it_fd = fd_maps.find(events[i].data.fd);
                    std::cout << "Client with an event :" << events[i].data.fd << std::endl;
                    if (events[i].events & EPOLLRDHUP) {
                        std::cout << "peer shutdown detected\n";
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                        continue;
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
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd , NULL);
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
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd , NULL);
                            close(events[i].data.fd);
                            fd_maps.erase(events[i].data.fd);
                            continue ;
                        }
                }
            }
        }
        }
}