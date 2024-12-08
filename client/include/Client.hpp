#pragma once

#include <string>
#include <netinet/in.h>
#include "SmartBuffer.hpp"
#include "Protocol.hpp"

class Client {
public:
    Client(const std::string &serverAddress, int serverPort);

    ~Client();

    void init();

    void connectTCP();

    void connectUDP();

    void disconnectTCP();

    void disconnectUDP();

    void run();

private:
    int tcpSocket;
    int udpSocket;
    sockaddr_in serverTcpAddr;
    sockaddr_in serverUdpAddr;

    void initTcpSocket();

    void initUdpSocket();

    void sendTcpBuffer(SmartBuffer &buffer);

    void sendUdpBuffer(SmartBuffer &buffer);

    SmartBuffer receiveTcpBuffer();

    SmartBuffer receiveUdpBuffer();

    void handleInput();

    void printResponse(SmartBuffer &buffer);
};
