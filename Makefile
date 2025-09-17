# SyntheticCoin (SYNC) Makefile
# The Small Miner's Cryptocurrency

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -I./src -I/usr/include
LIBS = -lssl -lcrypto -lpthread -lboost_system -lboost_filesystem -lboost_program_options -lboost_thread

# Source files
PODD_SRCS = src/podd/device_verifier.cpp
MINING_SRCS = src/mining/reward_calculator.cpp
DAEMON_SRCS = src/syncd.cpp
CLI_SRCS = src/sync-cli.cpp

# Object files
PODD_OBJS = $(PODD_SRCS:.cpp=.o)
MINING_OBJS = $(MINING_SRCS:.cpp=.o)
DAEMON_OBJS = $(DAEMON_SRCS:.cpp=.o)
CLI_OBJS = $(CLI_SRCS:.cpp=.o)

# Targets
all: syncd sync-cli

syncd: $(DAEMON_OBJS) $(PODD_OBJS) $(MINING_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "✓ Built syncd daemon"

sync-cli: $(CLI_OBJS) $(PODD_OBJS) $(MINING_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "✓ Built sync-cli"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f src/*.o src/*/*.o syncd sync-cli
	@echo "✓ Cleaned build files"

test: syncd sync-cli
	@echo "Running basic tests..."
	./sync-cli help
	./syncd --version
	@echo "✓ Basic tests passed"

install: syncd sync-cli
	@echo "Installing to /usr/local/bin..."
	@sudo cp syncd /usr/local/bin/
	@sudo cp sync-cli /usr/local/bin/
	@echo "✓ Installation complete"

run-example:
	@echo "=== Running Example Reward Calculation ==="
	./syncd --showreward
	@echo ""
	@echo "=== Calculating rewards for different hashrates ==="
	./sync-cli calcreward 0.5
	@echo ""
	./sync-cli calcreward 5
	@echo ""
	./sync-cli calcreward 50
	@echo ""
	./sync-cli calcreward 150

.PHONY: all clean test install run-example
