#include "multplixing.hpp"

#define MAX_CLIENTS 

void        multplixing::lanch_server()
{
    serverSocket = 0;

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

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in sock_info;

    sock_info.sin_family = AF_INET;
    sock_info.sin_port = htons(8080);
    sock_info.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr *)&sock_info, sizeof(sock_info))) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    // bash nchof wash l address w lport t assiaynaw mzyan :
                    // socklen_t len = sizeof(sock_info);
                    // if (getsockname(serverSocket, (struct sockaddr *)&sock_info, &len) == -1) {
                    //     perror("Getsockname failed");
                    //     exit(EXIT_FAILURE);
                    // }
                    // char ip[INET_ADDRSTRLEN];
                    // inet_ntop(AF_INET, &(sock_info.sin_addr), ip, INET_ADDRSTRLEN);
                    // printf("Local address: %s\n", ip);
                    // printf("Local port: %d\n", ntohs(sock_info.sin_port));
                    // close(serverSocket);
    /////////////////////////////////////////////////////////////////////

    if (listen(serverSocket, 5)) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    int flag;
    if ((flag = fcntl(serverSocket, F_GETFL, 0)) < 0) {
        perror("Failed getting file status");
        exit(EXIT_FAILURE);
    }
    flag |= O_NONBLOCK;
    if (fcntl(serverSocket, F_SETFL, flag) < 0) {
        perror("Failed setting file status");
        exit(EXIT_FAILURE);
    }
    
}