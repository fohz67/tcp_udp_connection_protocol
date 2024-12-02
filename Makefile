CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iclient/include -Iserver/include
CLIENT_SRC = client/src/main.cpp client/src/Client.cpp client/src/Protocol.cpp
SERVER_SRC = server/src/main.cpp server/src/Server.cpp server/src/Protocol.cpp
CLIENT_BIN = bin/client
SERVER_BIN = bin/server

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@

$(SERVER_BIN): $(SERVER_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf bin

.PHONY: all clean
