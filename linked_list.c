#include <stdlib.h>

#include "linked_list.h"

// Linked list helper functions

// Create node
node* create_node(char line[1024]) {
	// Malloc space for new node
	node* new_node = malloc(sizeof(node));

	// Make sure we didn't run out of memory
	if (new_node == NULL) {
		return NULL;
	}
	
	// Set the node's data
	for (int i = 0; i < 1024; i++) {
		new_node->line[i] = line[i];
	} 

	// Set the node's "next" pointer
	new_node->next = NULL;

	// Return pointer to new node we just created
	return new_node;
}
