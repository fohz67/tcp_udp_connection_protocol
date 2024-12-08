#include "Client.hpp"
#include <iostream>
#include <thread>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>

enum CALLBACKS {
    ROOM_CREATED,
    ROOM_JOINED,
    ROOM_DELETED,
    PLAYER_NOT_FOUND,
    ROOM_NOT_FOUND,
    PERMISSION_DENIED,
    ROOM_FULL,
};

Client::Client(const std::string &serverAddress, int serverPort)
        : tcpSocket(-1), udpSocket(-1) {
    serverTcpAddr.sin_family = AF_INET;
    serverTcpAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverAddress.c_str(), &serverTcpAddr.sin_addr);

    serverUdpAddr.sin_family = AF_INET;
    serverUdpAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverAddress.c_str(), &serverUdpAddr.sin_addr);
}

Client::~Client() {
    disconnectTCP();
    disconnectUDP();
}

void Client::init() {
    initTcpSocket();
    initUdpSocket();
}

void Client::initTcpSocket() {
    tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket == -1) {
        throw std::runtime_error("Failed to create TCP socket");
    }
}

void Client::initUdpSocket() {
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        throw std::runtime_error("Failed to create UDP socket");
    }
}

void Client::connectTCP() {
    if (connect(tcpSocket, (struct sockaddr *) &serverTcpAddr, sizeof(serverTcpAddr)) < 0) {
        throw std::runtime_error("Failed to connect to TCP server");
    }
    std::cout << "[Client] Connected to TCP server." << std::endl;
}

void Client::connectUDP() {
    std::cout << "[Client] UDP initialized." << std::endl;
}

void Client::disconnectTCP() {
    if (tcpSocket != -1) {
        close(tcpSocket);
        tcpSocket = -1;
    }
}

void Client::disconnectUDP() {
    if (udpSocket != -1) {
        close(udpSocket);
        udpSocket = -1;
    }
}

void Client::sendTcpBuffer(SmartBuffer &buffer) {
    uint32_t bufferSize = buffer.getSize();
    if (send(tcpSocket, buffer.getBuffer(), bufferSize, 0) < 0) {
        throw std::runtime_error("[Client] Failed to send TCP message.");
    }
}

void Client::sendUdpBuffer(SmartBuffer &buffer) {
    uint32_t bufferSize = buffer.getSize();
    if (sendto(udpSocket, buffer.getBuffer(), bufferSize, 0,
               (struct sockaddr *) &serverUdpAddr, sizeof(serverUdpAddr)) < 0) {
        throw std::runtime_error("[Client] Failed to send UDP message.");
    }
}

SmartBuffer Client::receiveTcpBuffer() {
    char rawBuffer[1024];
    int bytesReceived = recv(tcpSocket, rawBuffer, sizeof(rawBuffer), 0);
    if (bytesReceived <= 0) {
        throw std::runtime_error("[Client] Failed to receive TCP message.");
    }

    SmartBuffer buffer;
    buffer.inject(reinterpret_cast<uint8_t *>(rawBuffer), bytesReceived);
    return buffer;
}

SmartBuffer Client::receiveUdpBuffer() {
    char rawBuffer[1024];
    socklen_t serverLen = sizeof(serverUdpAddr);
    int bytesReceived = recvfrom(udpSocket, rawBuffer, sizeof(rawBuffer), 0,
                                 (struct sockaddr *) &serverUdpAddr, &serverLen);
    if (bytesReceived <= 0) {
        throw std::runtime_error("[Client] Failed to receive UDP message.");
    }

    SmartBuffer buffer;
    buffer.inject(reinterpret_cast<uint8_t *>(rawBuffer), bytesReceived);
    return buffer;
}

void Client::handleInput() {
    while (true) {
        std::cout << "\nEnter OpCode (1: CreateRoom, 2: JoinRoom, 3: DeleteRoom, 0: Exit): ";
        int opCode;
        std::cin >> opCode;

        if (opCode == 0) {
            std::cout << "[Client] Exiting input handler." << std::endl;
            break;
        }

        SmartBuffer buffer;

        switch (opCode) {
            case Protocol::OpCode::CREATE_ROOM: {
                int32_t playerId;
                int16_t capacity;
                int16_t isPublic;

                std::cout << "Enter Player ID: ";
                std::cin >> playerId;
                std::cout << "Enter Room Capacity: ";
                std::cin >> capacity;
                std::cout << "Is Public (1 for true, 0 for false): ";
                std::cin >> isPublic;

                buffer << int16_t(Protocol::OpCode::CREATE_ROOM) << playerId << int16_t(capacity) << int16_t(isPublic);
                sendTcpBuffer(buffer);
                break;
            }
            case Protocol::OpCode::JOIN_ROOM: {
                std::string roomCode;
                int32_t playerId;

                std::cout << "Enter Room Code: ";
                std::cin >> roomCode;
                std::cout << "Enter Player ID: ";
                std::cin >> playerId;

                buffer << int16_t(Protocol::OpCode::JOIN_ROOM) << roomCode << playerId;
                sendTcpBuffer(buffer);
                break;
            }
            case Protocol::OpCode::DELETE_ROOM: {
                std::string roomCode;
                int32_t playerId;

                std::cout << "Enter Room Code: ";
                std::cin >> roomCode;
                std::cout << "Enter Player ID: ";
                std::cin >> playerId;

                buffer << int16_t(Protocol::OpCode::DELETE_ROOM) << roomCode << playerId;
                sendTcpBuffer(buffer);
                break;
            }
            default:
                std::cout << "[Client] Invalid OpCode entered." << std::endl;
                continue;
        }

        try {
            SmartBuffer response = receiveTcpBuffer();
            printResponse(response);
        } catch (const std::exception &e) {
            std::cerr << "[Client] Error while waiting for response: " << e.what() << std::endl;
        }
    }
}

void Client::printResponse(SmartBuffer &buffer) {
    int16_t opCode;
    buffer >> opCode;

    std::cout << "[Client] Received OpCode: " << int(opCode) << std::endl;

    switch (opCode) {
        case Protocol::OpCode::CREATE_ROOM_CALLBACK: {
            int16_t statusCode;
            buffer >> statusCode;

            if (statusCode == CALLBACKS::ROOM_CREATED) {
                std::string roomCode;
                buffer >> roomCode;
                std::cout << "[Client] Room successfully created. Code: " << roomCode << std::endl;
            } else {
                std::cout << "[Client] Failed to create room. Error Code: " << int(statusCode) << std::endl;
            }
            break;
        }
        case Protocol::OpCode::JOIN_ROOM_CALLBACK: {
            int16_t statusCode;
            buffer >> statusCode;

            if (statusCode == CALLBACKS::ROOM_JOINED) {
                std::cout << "[Client] Successfully joined the room." << std::endl;
            } else {
                std::cout << "[Client] Failed to join room. Error Code: " << int(statusCode) << std::endl;
            }
            break;
        }
        case Protocol::OpCode::DELETE_ROOM_CALLBACK: {
            int16_t statusCode;
            buffer >> statusCode;

            if (statusCode == CALLBACKS::ROOM_DELETED) {
                std::cout << "[Client] Room successfully deleted." << std::endl;
            } else {
                std::cout << "[Client] Failed to delete room. Error Code: " << int(statusCode) << std::endl;
            }
            break;
        }
        default:
            std::cout << "[Client] Unknown OpCode received: " << int(opCode) << std::endl;
            break;
    }
}

void Client::run() {
    std::thread([this]() {
        while (true) {
            try {
                SmartBuffer response = receiveTcpBuffer();
                printResponse(response);
            } catch (const std::exception &e) {
                std::cerr << "[Client] Error in TCP: " << e.what() << std::endl;
                break;
            }
        }
    }).detach();

    handleInput();
}
