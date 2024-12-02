#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Server::Server(int port) : port(port), tcpSocket(-1), udpSocket(-1) {
    initTcpSocket();
    initUdpSocket();
}

Server::~Server() {
    closeSockets();
}

void Server::start() {
    std::thread udpThread(&Server::handleUdpMessages, this);
    udpThread.detach();

    handleTcpConnections();
}

void Server::initTcpSocket() {
    tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket == -1) throw std::runtime_error("Failed to create TCP socket");

    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.s_addr = INADDR_ANY;
    tcpAddr.sin_port = htons(port);

    if (bind(tcpSocket, (struct sockaddr*)&tcpAddr, sizeof(tcpAddr)) < 0)
        throw std::runtime_error("Bind failed for TCP socket");

    if (listen(tcpSocket, 3) < 0)
        throw std::runtime_error("Listen failed for TCP socket");
}

void Server::initUdpSocket() {
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) throw std::runtime_error("Failed to create UDP socket");

    udpAddr.sin_family = AF_INET;
    udpAddr.sin_addr.s_addr = INADDR_ANY;
    udpAddr.sin_port = htons(port);

    if (bind(udpSocket, (struct sockaddr*)&udpAddr, sizeof(udpAddr)) < 0)
        throw std::runtime_error("Bind failed for UDP socket");
}

void Server::handleTcpConnections() {
    std::cout << "TCP server listening on port " << port << std::endl;

    while (true) {
        int clientSocket = accept(tcpSocket, nullptr, nullptr);
        if (clientSocket < 0) continue;

        std::cout << "New TCP client connected" << std::endl;
        clientThreads.emplace_back(&Server::handleTcpClient, this, clientSocket);
    }
}

void Server::handleTcpClient(int clientSocket) {
    char buffer[1024] = {0};

    while (true) {
        int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            std::cerr << "TCP client disconnected" << std::endl;
            close(clientSocket);
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << "TCP Received: " << buffer << std::endl;

        std::string response = "TCP Message received: " + std::string(buffer);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
}

void Server::handleUdpMessages() {
    char buffer[1024] = {0};
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    std::cout << "UDP server listening on port " << port << std::endl;

    while (true) {
        int bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0,
                                 (struct sockaddr*)&clientAddr, &addrLen);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "UDP Received: " << buffer << std::endl;

            std::string response = "UDP Message received: " + std::string(buffer);
            sendto(udpSocket, response.c_str(), response.size(), 0,
                   (struct sockaddr*)&clientAddr, addrLen);
        }
    }
}

void Server::closeSockets() {
    close(tcpSocket);
    close(udpSocket);

    for (auto& thread : clientThreads) {
        if (thread.joinable()) thread.join();
    }
}
