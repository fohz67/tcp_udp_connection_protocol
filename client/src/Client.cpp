#include "Client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client(const std::string &serverAddress, int serverPort)
        : tcpSocket(-1), udpSocket(-1), isTcpConnected(false), isUdpInitialized(false) {
    serverTcpAddr.sin_family = AF_INET;
    serverTcpAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverAddress.c_str(), &serverTcpAddr.sin_addr);

    serverUdpAddr.sin_family = AF_INET;
    serverUdpAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverAddress.c_str(), &serverUdpAddr.sin_addr);
}

Client::~Client() {
    closeSockets();
}

void Client::init() {
    initTcpSocket();
    initUdpSocket();
}

void Client::initTcpSocket() {
    tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket == -1) throw std::runtime_error("Failed to create TCP socket");
}

void Client::initUdpSocket() {
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) throw std::runtime_error("Failed to create UDP socket");
    isUdpInitialized = true;
}

void Client::connectTCP() {
    if (connect(tcpSocket, (struct sockaddr *) &serverTcpAddr, sizeof(serverTcpAddr)) < 0)
        throw std::runtime_error("Failed to connect to TCP server");

    isTcpConnected = true;
    std::cout << "Connected to TCP server" << std::endl;

    handleTcpLoop();
}

void Client::connectUDP() {
    if (!isUdpInitialized) throw std::runtime_error("UDP socket not initialized");
    std::cout << "Ready to communicate with UDP server" << std::endl;

    handleUdpLoop();
}

void Client::disconnectTCP() {
    if (!isTcpConnected) {
        std::cout << "TCP socket is not connected" << std::endl;
        return;
    }

    close(tcpSocket);
    isTcpConnected = false;
    std::cout << "Disconnected from TCP server" << std::endl;
}

void Client::disconnectUDP() {
    if (!isUdpInitialized) {
        std::cout << "UDP socket is not initialized" << std::endl;
        return;
    }

    close(udpSocket);
    isUdpInitialized = false;
    std::cout << "UDP socket closed" << std::endl;
}

void Client::handleTcpLoop() {
    char buffer[1024] = {0};
    std::string message;

    while (isTcpConnected) {
        std::cout << "Enter TCP message (type 'exit' to disconnect): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            disconnectTCP();
            break;
        }

        send(tcpSocket, message.c_str(), message.size(), 0);

        int bytesRead = read(tcpSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "TCP Server: " << buffer << std::endl;
        }
    }
}

void Client::handleUdpLoop() {
    char buffer[1024] = {0};
    sockaddr_in fromAddr;
    socklen_t addrLen = sizeof(fromAddr);
    std::string message;

    while (isUdpInitialized) {
        std::cout << "Enter UDP message (type 'exit' to stop): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            disconnectUDP();
            break;
        }

        sendto(udpSocket, message.c_str(), message.size(), 0,
               (struct sockaddr *) &serverUdpAddr, sizeof(serverUdpAddr));

        int bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0,
                                 (struct sockaddr *) &fromAddr, &addrLen);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "UDP Server: " << buffer << std::endl;
        }
    }
}

void Client::closeSockets() {
    if (isTcpConnected) disconnectTCP();
    if (isUdpInitialized) disconnectUDP();
}
