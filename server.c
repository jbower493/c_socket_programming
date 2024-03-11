// https://pubs.opengroup.org/onlinepubs/009604499/basedefs/netinet/in.h.html
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
// https://pubs.opengroup.org/onlinepubs/009604499/basedefs/sys/socket.h.html
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8080
#define BACKLOG 10

/*
* Helpers
*/
typedef struct http_request_headers {
	char *accept;
	char *host;
	char *user_agent;
} http_request_headers;

typedef struct http_request {
    char method[10];
    char path[1024];
    char protocol[20];
    http_request_headers headers;
} http_request;

// Returns 0 if true
int starts_with(char *string, char *target) {
    return strncmp(string, target, strlen(target));
}

// Returns 0 if successful
int parse_response(char request_buffer[10240], http_request *request_ptr) {
    char *token;

    token = strtok(request_buffer, "\n");

    sscanf(token, "%s %s %s", request_ptr->method, request_ptr->path, request_ptr->protocol);

    while (token != NULL) {
        if (starts_with(token, "Host:") == 0) {
            char *host = token + strlen("Host: ");
            request_ptr->headers.host = host;
        } else if (starts_with(token, "Accept:") == 0) {
            char *accept = token + strlen("Accept: ");
            request_ptr->headers.accept = accept;
        } else if (starts_with(token, "User-Agent:") == 0) {
            char *ua = token + strlen("User-Agent: ");
            request_ptr->headers.user_agent = ua;
        }

        token = strtok(NULL, "\n");
    }

    return 0;
}

/*
* Main
*/
int main() {
	/***** VARIABLES *****/
	// Boolean to use for setting socket option values
	int socket_option_value = 1;

	// Server internet socket address
	struct sockaddr_in server_address;
	// Protocal, IPv4
	server_address.sin_family = AF_INET;
	// Port number, in "network byte order" (host-to-network short)
	server_address.sin_port = htons(PORT);
	// Internet address, in "network byte order" (host-to-network long). Use INADDR_LOOPBACK for localhost, use INADDR_ANY for any address
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	/***** PROGRAM START *****/
    // Define a socket
	int server_socket = socket(
		// Domain (int), use AF_INET for communicating between processes on different hosts via IPV4
		AF_INET,
		// Communication type, use SOCK_STREAM for TCP, SOCK_DGRAM for UDP
		SOCK_STREAM,
		// Protocol (int), use 0 for IP (internet protocol)
		0
	);

	// Exit if create socket failed
	if (server_socket < 0) {
		printf("Failed to create socket\n");
		return 1;
	}

	// Set socket option (useful for socket reuse)
	int set_socket_option_result = setsockopt(
		// Socket, The socket to set options for
		server_socket,
		// Level, "Socket" level or "protocol" level
		SOL_SOCKET,
		// Option name, the name of the single option to set
		SO_REUSEADDR,
		// Option value (const void *), "const void *" means a pointer to some memory that should not be modified
		&socket_option_value,
		// Option length
		sizeof(int)
	);
	
	// Exit if set socket option failed
	if (set_socket_option_result < 0) {
		printf("Failed to set socket option\n");
		return 1;
	}

	int socket_bind_result = bind(
		// Socket (descriptor)
		server_socket,
		// The address to bind the socket to (protocol, port and address
		(struct sockaddr *)&server_address,
		// Length of the address in bytes
		sizeof(server_address)
	);

	// Exit if socket bind failed
	if (socket_bind_result < 0) {
		printf("Failed to bind socket\n");
		return 1;
	}

	// Start the socket listening for incoming connections
	int listen_result = listen(
		// Socket to listen with
		server_socket,
		// Backlog (max connections, socket will get an error if it recieves a connection when the current num of connections is already at the max
		BACKLOG
	);

	// Exit if socket fails to listen
	if (listen_result < 0) {
		printf("Socket failed to listen\n");
		return 1;
	}

	// Extract the first connection request in the pending connections queue and return a file descriptor to the connected client socket
	int connected_client_socket = accept(
		// Socket, listening socket on the server that we've already created, bound, and listened with
		server_socket,
		// address, null pointer or a pointer to a sockaddr structure to hold the address of the connecting socket
		NULL,
		// address_len, pointer to a variable (socklen_t) to hold length of the supplied sockaddr structure supplied in arg2, or null pointer if none was provided
		NULL
	);

	// Exit if accept failed
	if (connected_client_socket < 0) {
		printf("Failded to accept connection\n");
		return 1;
	}

	// Read the request data
	char request_buffer[10240];
	int num_of_bytes_read = read(
		// Client socket to read from
		connected_client_socket,
		// Buffer into which to write the read data
		request_buffer,
		// Size to read
		10240
	);

	// Exit if read failed
	if (num_of_bytes_read < 0) {
		printf("Faild to read request data\n");
		return 1;
	}

    http_request request_data;
	int parse_response_result = parse_response(request_buffer, &request_data);

    printf("Path: %s\n", request_data.path);
    printf("Method: %s\n", request_data.method);
    printf("Protocol: %s\n", request_data.protocol);
    printf("Accept: %s\n", request_data.headers.accept);
    printf("User-Agent: %s\n", request_data.headers.user_agent);
    printf("Host: %s\n", request_data.headers.host);

	// Respond to client
	// char * response = "Hey client, how's it going";
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\n\n<h1>Mate</h1>\n";

	int num_of_bytes_sent = send(
		// When sending a response from the server we need to use the client socket rather than the server socket
		connected_client_socket,
		// message
		response,
		// message size
		strlen(response),
		// flags
		0
	);

	if (num_of_bytes_sent < 0) {
		printf("Failed to respond to client");
		return 1;
	}
	

	// Close the connected socket
	close(connected_client_socket);
	// Close the listening socket
	close(server_socket);

	return 0;
}
