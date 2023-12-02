#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main() {
	// Server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	// Convert socket address from plain text into numberic binary
	int address_text_to_binary_result = inet_pton(
		// Address family (IPv4 or IPv6)
		AF_INET,
		// String being passed in
		"127.0.0.1",
		// dst, buffer in which to store the numberic address. For this we can pass in the .sin_addr of the server address
		&server_address.sin_addr
	);

	// Exit if the address conversion failed. Here we have to check if it's <= 0, because 0 is returned if the input string is not valid, and -1 for any other error
	if (address_text_to_binary_result <= 0) {
		printf("Address text to binary conversion failed\n");
		return 1;
	}

	// Create the client socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Exit if creation failed
	if (client_socket < 0) {
		printf("Client socket creation failed\n");
		return 1;
	}

	// Attempt to connect to the server
	int connected = connect(
		// The client socket we're using to connect to the server with
		client_socket,
		// Address, address of the serer socket to connect to
		(struct sockaddr *)&server_address,
		// Size in bytes of the server address
		sizeof(server_address)
	);

	// Exit if connection fails
	if (connected < 0) {
		printf("Connection to server failed\n");
		return 1;
	}

	// Send a message to the server
	char * message = "Hello from the client socket once again";

	int num_of_bytes_sent = send(
		// Client socket to send with
		client_socket,
		// buffer, pointer to the char * message to send
		message,
		// buffer size
		strlen(message),
		// flage
		0
	);

	if (num_of_bytes_sent < 0) {
		printf("Failed to sent message");
		return 1;
	}
		
	// Read the response from the server
	char buffer[1024];
	int num_of_bytes_read = read(client_socket, buffer, 1024);

	// Exit if failed to read response
	if (num_of_bytes_read < 0) {
		printf("Failed to read response from server");
		return 1;
	}

	for (int i = 0; i < num_of_bytes_read; i++) {
		printf("Char %i: %c\n", i, buffer[i]);
	}  

	printf("Client success\n");
    return 0;
}
