#include <iostream>
#include "Server.hpp"

int main() {
    const int PORT = 8080;

    try {
        Server server(PORT);
        server.start();
    } catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
