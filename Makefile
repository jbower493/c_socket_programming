server: server.c
	gcc -o server server.c linked_list.c parse_response.c

client: client.c
	gcc -o client client.c
