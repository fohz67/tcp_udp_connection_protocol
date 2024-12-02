#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Server::Server(int port) : serverSocket(-1), port(port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Bind failed");
    }
}

Server::~Server() {
    close(serverSocket);
    for (auto &thread: clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void Server::start() {
    if (listen(serverSocket, 3) < 0) {
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        std::cout << "New client connected\n";
        clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
    }
}

void Server::handleClient(int clientSocket) {
    char buffer[1024] = {0};

    while (true) {
        int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            std::cerr << "Client disconnected.\n";
            close(clientSocket);
            break;
        }
        buffer[bytesRead] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        std::string response = "Message received: ";
        response += buffer;
        send(clientSocket, response.c_str(), response.size(), 0);
    }
}
