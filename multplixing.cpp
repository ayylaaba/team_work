#include "multplixing.hpp"

void        multplixing::lanch_server()
{
    serverSocket = 0;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    int sp = 1;
    // Bind the socket
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &sp, sizeof(sp));
    if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1) 
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port 8080...\n";

    epoll_fd = epoll_create(EPOLL_CLOEXEC);
    struct epoll_event envts;
    envts.data.fd = serverSocket;
    envts.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &envts);

}