#include <iostream>
#include "Client.hpp"

int main() {
    const std::string SERVER_ADDRESS = "127.0.0.1";
    const int SERVER_PORT = 8080;

    try {
        Client client(SERVER_ADDRESS, SERVER_PORT);
        client.connectToServer();

        // Exemple d'envoi/réception TCP
        client.sendTcpMessage("Hello via TCP");
        client.receiveTcpMessage();

        // Exemple d'envoi UDP
        client.sendUdpMessage("Hello via UDP");
        client.receiveUdpMessage();

    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}