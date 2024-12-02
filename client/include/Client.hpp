#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <string>

class Client {
public:
    Client(const std::string& serverAddress, int serverPort);
    ~Client();

    void init();
    void connectTCP();
    void connectUDP();
    void disconnectTCP();
    void disconnectUDP();

private:
    int tcpSocket;
    int udpSocket;

    sockaddr_in serverTcpAddr;
    sockaddr_in serverUdpAddr;

    bool isTcpConnected;
    bool isUdpInitialized;

    void initTcpSocket();
    void initUdpSocket();
    void closeSockets();
    void handleTcpLoop();
    void handleUdpLoop();
};

#endif // CLIENT_HPP