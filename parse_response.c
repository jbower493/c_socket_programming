#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

char* accept_key = "Accept:";
char* user_agent_key = "User-Agent:";
char* host_key = "Host:";

typedef struct http_headers {
	char request[1024];
	char accept[1024];
	char host[1024];
	char user_agent[1024];
} http_headers;

int parse_response(char request_buffer[10240], int num_of_bytes_read) {

        // Bail out if there is no data
        if (num_of_bytes_read <= 0) {
                return 0;
        }

        // Create linked list
        node* head = NULL;
        node* temp = NULL;

        // Create first node
        temp = create_node("");

        // Index to write to in the line
        int line_index = 0;

        for (int i = 0; i < num_of_bytes_read; i++) {
                // Write the current char into the first linked list node. When we hit a new line, create a new linked list node

                if (request_buffer[i] == '\n') {
                        // Add the current node to the start of the list
                        temp->next = head;
                        head = temp;

                        // Create the next node to be written to
                        temp = create_node("");
                        // Reset line index
                        line_index = 0;
                } else {
                        // Write the char into the linked list node
                        temp->line[line_index] = request_buffer[i];
                        temp->line_length = line_index + 1;

                        // Increment line index
                        line_index++;
                }
        }

        // Convert linked list of headers into a struct with the headers we're looking for
        temp = head;
        http_headers* request_headers;

        while (temp != NULL) {
                // Allocate the header into our headers struct
                int first_space_index;
                for (int i = 0; i < temp->line_length; i++) {
                        if (temp->line[i] == ' ') {
                                first_space_index = i;
                                break;
                        }
                }

                // Extract the header keys and values
                char header_key[first_space_index - 1];
                char header_value[temp->line_length - first_space_index - 1];
                for (int i = 0; i < temp->line_length; i++) {
                        if (i < first_space_index) {
                                header_key[i] = temp->line[i];
                        } else if (i > first_space_index) {
                                header_value[i - first_space_index - 1] = temp->line[i];
                        }
                }

// ERROR: segfault is somewhere in here
                // Write the header value into the request headers struct
                for (int i = first_space_index + 1; i < temp->line_length; i++) {
                        if (strcmp(header_key, accept_key) == 0) {
                                request_headers->accept[i - first_space_index - 1] = temp->line[i];
                        } else if (strcmp(header_key, host_key) == 0) {
                                request_headers->host[i - first_space_index - 1] = temp->line[i];
                        }
                }

                temp = temp->next;
        }


        // Free linked list
        while (head != NULL) {
                temp = head;
                head = temp->next;
                free(temp);
        }

        return 1;
}
