#include <iostream>
#include <exception>
#include <string>

#include <sys/socket.h>
#include <netinet/ip.h> //for INADDR_ANY
#include <arpa/inet.h> //for inet_pton

constexpr std::string IP{"192.168.1.70"}; //c++20 required to use constexpr std::string
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

}

int main() {
	try {
		client_socket();
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}
