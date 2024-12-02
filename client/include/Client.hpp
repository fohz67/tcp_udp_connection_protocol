#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netinet/in.h>

class Client {
public:
    Client(const std::string& serverAddress, int serverPort);
    ~Client();

    void connectToServer();
    void run();

private:
    std::string serverAddress;
    int serverPort;
    int clientSocket;
    sockaddr_in serverAddr;

    void sendMessage(const std::string& message);
    void receiveMessage();
};

#endif // CLIENT_HPP
