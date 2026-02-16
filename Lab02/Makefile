BUILD_DIR = build
CC = g++


all: $(BUILD_DIR)/server $(BUILD_DIR)/client

$(BUILD_DIR)/server: server.cpp $(BUILD_DIR)
	$(CC) server.cpp -o $(BUILD_DIR)/server -Wall -std=c++20

$(BUILD_DIR)/client: client.cpp $(BUILD_DIR)
	$(CC) client.cpp -o $(BUILD_DIR)/client -Wall -std=c++20

$(BUILD_DIR):
	mkdir $(BUILD_DIR)
