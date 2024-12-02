#include <iostream>
#include "Client.hpp"

int main() {
    const std::string SERVER_ADDRESS = "127.0.0.1";
    const int SERVER_PORT = 8080;

    try {
        Client client(SERVER_ADDRESS, SERVER_PORT);

        client.init();
        client.connectTCP();
        client.connectUDP();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
