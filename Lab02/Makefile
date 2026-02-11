BUILD_DIR = build
CC = g++


all: $(BUILD_DIR)/server $(BUILD_DIR)/client

$(BUILD_DIR)/server: server.cpp
	$(CC) server.cpp -o $(BUILD_DIR)/server -Wall -std=c++20

$(BUILD_DIR)/client: client.cpp
	$(CC) client.cpp -o $(BUILD_DIR)/client -Wall -std=c++20
