# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iclient/include -Iserver/include

# Source files and binaries
CLIENT_SRC = client/src/main.cpp client/src/Client.cpp client/src/SmartBuffer.cpp
CLIENT_BIN = bin/client

# Default rule
all: $(CLIENT_BIN)

# Build the client binary
$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built $(CLIENT_BIN)"

# Clean the binary files
clean:
	rm -rf bin
	@echo "Cleaned binary files"

# Full clean
fclean: clean
	@echo "Fully cleaned project"

# Rebuild the project
re: fclean all

.PHONY: all clean fclean re