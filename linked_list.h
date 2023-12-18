

typedef struct node {
	char line[1024];
	int line_length;
	struct node* next;
} node;

node* create_node(char line[1024]);
