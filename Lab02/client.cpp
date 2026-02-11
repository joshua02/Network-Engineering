#include <iostream>
#include <exception>
#include <string>
#include <array>
#include <sstream>
#include <thread>

#include <sys/socket.h>
#include <netinet/ip.h> //for INADDR_ANY
#include <arpa/inet.h> //for inet_pton

const std::string IP{"192.168.1.70"};
constexpr int PORT{8080};


void client_socket() {

	//Create client socket
	int sock{socket(AF_INET, SOCK_STREAM, 0)};
	if (sock == -1) {
		throw std::runtime_error{"failed to create socket"};
	}

	// Fill out server address struct
	struct sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, IP.c_str(), &server_addr.sin_addr) == -1) {
		throw std::runtime_error{"failed to convert IP"};
	}

	//Attempt connection to server
	if(connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
		throw std::runtime_error{"failed to connect"};
	}

	std::cout << "connected!\n";


	auto rx_task = [&]() {
		std::array<char, 128> rx_buffer{};
		while (true) {
			if(recv(sock, (void*) &rx_buffer, sizeof(rx_buffer), 0) > 0) {
				std::stringstream rx{rx_buffer.data()};
				int a{};
				rx >> a;
				std::cout << "received: " << a << '\n';
			}
		}
	};

	auto tx_task = [&]() {
		while(true) {
			std::array<char, 128> tx_buffer{};
			std::string input{};
			std::cin >> input;
			if(input == "RATE") {
				int value{};
				std::cin >> value;
				std::cout << "Sending command: RATE " << value << '\n';
				std::string command{"RATE " + std::to_string(value)};
				std::copy(command.begin(), command.end(), tx_buffer.begin());
				send(sock, (void*) &tx_buffer, sizeof(tx_buffer), 0);
			} else if(input == "SET") {
				int value{};
				std::cin >> value;
				std::cout << "Sending command: SET " << value << '\n';
				std::string command{"SET " + std::to_string(value)};
				std::copy(command.begin(), command.end(), tx_buffer.begin());
				send(sock, (void*) &tx_buffer, sizeof(tx_buffer), 0);
			} else {
				std::cout << "No command parsed: " << input << '\n';
			}
		}
	};

	std::jthread rx_thread{rx_task};
	std::jthread tx_thread{tx_task};
}

int main() {
	try {
		client_socket();
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}
