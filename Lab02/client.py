import socket
import threading

HOST = "146.163.133.27"
PORT = 8080


def rx_loop(sock):
    while True:
        data = sock.recv(128)
        print("Received: " + data.decode("ascii"))

def tx_loop(sock):
    while True:
        # no input validation
        input_text = input("")
        sock.send(input_text.encode("ascii"))

def main():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((HOST, PORT))
    print(f"Connected to {HOST}:{PORT}")

    # spawn a new thread to handle recieving
    recv_thread = threading.Thread(target=rx_loop, args=(client_socket,))
    recv_thread.start()

    # handle transmitting on main thread
    tx_loop(client_socket)

if __name__ == "__main__":
    main()