#include "Client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client(const std::string& serverAddress, int serverPort)
        : serverAddress(serverAddress), serverPort(serverPort), clientSocket(-1) {}

Client::~Client() {
    if (clientSocket != -1) {
        close(clientSocket);
    }
}

void Client::connectToServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid server address");
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Connection to server failed");
    }

    std::cout << "Connected to server at " << serverAddress << ":" << serverPort << std::endl;
}

void Client::run() {
    std::string message;
    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        if (message == "exit") break;

        sendMessage(message);
        receiveMessage();
    }
}

void Client::sendMessage(const std::string& message) {
    send(clientSocket, message.c_str(), message.size(), 0);
}

void Client::receiveMessage() {
    char buffer[1024] = {0};
    int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (bytesRead <= 0) {
        throw std::runtime_error("Server disconnected");
    }
    buffer[bytesRead] = '\0';
    std::cout << "Server: " << buffer << std::endl;
}
