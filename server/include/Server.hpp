#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <thread>
#include <netinet/in.h>

class Server {
public:
    explicit Server(int port);
    ~Server();
    void start();

private:
    int serverSocket;
    int port;
    sockaddr_in serverAddr;
    std::vector<std::thread> clientThreads;

    void handleClient(int clientSocket);
};

#endif // SERVER_HPP
