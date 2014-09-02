#ifndef NET_H
#define NET_H
#include "../creature.h"

extern int listening_socket;
extern int client_socket;

typedef struct socket_node socket_node;

int list_altr;
struct socket_node{
	int sock;
	creature player;
	socket_node* next;
};


extern socket_node* server_sockets;

int client_connect(const char* ip, int port);
int setup_listener(int port);
int try_accept();
int read_packet(int socket, socket_node* s);
void cleanup_socket(int socket);

void server_listen(socket_node* node);
void server_update_clients();
void server_tile_update(tile* t, int x, int y);

int full_write(int sock, void* start, int len);

void list_insert(socket_node** node, int s);
void list_delete(socket_node** node, int s);

#endif
