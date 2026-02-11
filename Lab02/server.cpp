#include <iostream>
#include <exception>
#include <thread>
#include <chrono>
#include <string>
#include <array>

#include <sys/socket.h>
#include <netinet/ip.h> //for INADDR_ANY

constexpr int PORT{8080};

int create_server_socket() {

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

	return sock;
}

void rx_loop(const std::vector<int>& sockets, int& counter, int& count_inc) {
	std::array<char, 128> rx_buffer{};
	for (auto it = sockets.begin(); it != sockets.end(); it++) {
		if(recv(*it, (void*) &rx_buffer, sizeof(rx_buffer), MSG_DONTWAIT) > 0) {
			std::stringstream input{rx_buffer.data()};
			std::string command{};
			int value{};
			input >> command;
			input >> value;
			std::cout << "Received: " << command << " " << value << '\n';
			if (command == "RATE") {
				count_inc = value;
			}
			if(command == "SET") {
				counter = value;
			}
		}
	}
}

//Loop through connected clients, assumes client disconnect if it doesn't receive the message
void broadcast_to_sockets(std::vector<int>& sockets, int counter) {
	std::array<char, 128> tx_buffer{};
	std::string ascii_count{std::to_string(counter)};
	std::copy(ascii_count.begin(), ascii_count.end(), tx_buffer.begin());

	for (auto it = sockets.begin(); it != sockets.end(); ) {
		if(send(*it, (void*) &tx_buffer, sizeof(tx_buffer), MSG_NOSIGNAL) < 0) {
			close(*it);
			it = sockets.erase(it);
			std::cout << "Client disconnected!\n";
		} else {
			it++;
		}
	}
}



int main() {
	int server_sock{};
	bool accept_clients{true};
	std::vector<int> client_sockets{};
	int counter{};
	int count_inc{1};
	bool counter_enabled{true};

	auto count_task = [&]() {
		using namespace std::chrono_literals;
		while (counter_enabled) {
			rx_loop(client_sockets, counter, count_inc);
			counter += count_inc;
			broadcast_to_sockets(client_sockets, counter);
			std::this_thread::sleep_until(std::chrono::steady_clock::now() + 1000ms);
		}
	};

	auto accept_client_task = [&]() {
		while (accept_clients) {
			struct sockaddr_in client_addr{};
			socklen_t client_len = sizeof(client_addr);

			int client_sock{accept(server_sock, (struct sockaddr*) &client_addr, &client_len)};
			if (client_sock == -1) {
				throw std::runtime_error{"failed to accept client connection"};
			}
			std::cout << "Client connected!\n";
			client_sockets.push_back(client_sock);
		}
	};

	try {
		std::jthread count_thread{count_task};
		server_sock = create_server_socket();
		std::cout << "Listening on port " << PORT << '\n';
		std::jthread accept_client_thread{accept_client_task};
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	//TODO: close sockets
}
