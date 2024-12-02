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
    int tcpSocket;
    int udpSocket;
    int port;

    sockaddr_in tcpAddr;
    sockaddr_in udpAddr;

    std::vector<std::thread> clientThreads;

    void initTcpSocket();
    void initUdpSocket();

    void handleTcpConnections();
    void handleTcpClient(int clientSocket);
    void handleUdpMessages();

    void closeSockets();
};

#endif // SERVER_HPP