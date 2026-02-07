#include <iostream>
#include <exception>

#include <sys/socket.h>
#include <netinet/ip.h> //for INADDR_ANY

constexpr int PORT{8080};

void server_socket() {

	//Create server socket
	int sock{socket(AF_INET, SOCK_STREAM, 0)};
	if (sock == -1) {
		throw std::runtime_error{"failed to create socket"};
	}
	//Fill out server address struct, htons is needed to convert byte order
	struct sockaddr_in addr{AF_INET, htons(PORT), INADDR_ANY};

	//Tell socket what it's address is
	if(bind(sock, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		throw std::runtime_error{"failed to bind"};
	}

	//Tell socket it will be listening for other socket connections, maximum of 5 sockets queued
	if(listen(sock, 5) == -1) {
		throw std::runtime_error{"failed to listen"};
	}

	std::cout << "Listening on port " << PORT << '\n';

	//
	struct sockaddr_in client_addr{};
	socklen_t client_len = sizeof(client_addr);

	int client_sock{accept(sock, (struct sockaddr*) &client_addr, &client_len)};
	if (client_sock == -1) {
		throw std::runtime_error{"failed to accept client connection"};
	}

	std::cout << "Peer connected: " << client_len << '\n';


}

int main() {
	try {
		server_socket();
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}
