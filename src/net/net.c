#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "net.h"
#include "packet.h"
#include "../log.h"
#include "../world.h"

int listening_socket = -1;
int client_socket = -1;
socket_node* server_sockets = NULL;

//socket creation should probably be moved
int client_connect(const char* ip, int port){

	struct sockaddr_in serv_name;
	int status;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1){
		perror("Socket creation");
		return 1;
	}

	/* server address */
	serv_name.sin_family = AF_INET;
	inet_aton(ip, &serv_name.sin_addr);
	serv_name.sin_port = htons(port);

	/* connect to the server */
	status = connect(client_socket, (struct sockaddr*)&serv_name, sizeof(serv_name));
	if (status == -1){
		perror("Connection error");
		close(client_socket);
		client_socket=-1;
		return 1;
	}
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	return 0;
}

int setup_listener(int port){

	struct sockaddr_in my_name;
	int status;

	/* create a socket */
	listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_socket == -1){
		perror("Socket creation error");
		return 1;
	}

	/* server address  */
	my_name.sin_family = AF_INET;
	my_name.sin_addr.s_addr = INADDR_ANY;
	my_name.sin_port = htons(port);

	status = bind(listening_socket, (struct sockaddr*)&my_name, sizeof(my_name));
	if (status == -1){
		perror("Binding error");
		close(listening_socket);
		listening_socket=-1;
		return 1;
	}

	status = listen(listening_socket, 5);
	if (status == -1){
		perror("Listening error");
		close(listening_socket);
		listening_socket=-1;
		return 1;
	}

	fcntl(listening_socket, F_SETFL, O_NONBLOCK);

	return 0;
}

void cleanup_socket(int socket){
#ifdef SERVER
list_delete(&server_sockets,socket);
list_altr=1;
#else
client_socket=-1;
#endif
}

//returns true if a new connection is made
int try_accept(){

	struct sockaddr_in peer;
	socklen_t addrlen;
	int sock;

	sock = accept(listening_socket, (struct sockaddr*)&peer, &addrlen);
	if (sock != -1){
		list_insert(&server_sockets,sock);
		fcntl(sock, F_SETFL, O_NONBLOCK);
		info("New connection accepted");
		return 1;
	}

	return 0;
}

int read_packet(int socket, socket_node* s){

	if(socket == -1) return 1;

	int count;
	int total;
	packet_header head;

	count = read(socket,&head,sizeof(packet_header));

	if(count < 1)
		return 1;

	if(count != sizeof(packet_header)){
		warning("Fragmented header! %i Closing Connection",count);
		close(socket);
		cleanup_socket(socket);
		return 1;
	}
	void* packet= malloc(head.length);
	total=0;

	while(13){
		count= read(socket,packet+total,head.length-total);

		if(count == -1){
			if(errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			error("Error reading %i", errno);
			close(socket);
			cleanup_socket(socket);
			return 1;
		}

		total+=count;
		if(total>=head.length)
			break;
	}
	packet_handlers[head.type](s,packet,head.length);
	free(packet);
	return 0;
}

//TODO figure out why the server log is full of writing errors
int full_write(int sock, void* start, int len){
	int sent=0;
	int tmp;

	while(sent<len){
		//for apple support
#ifdef __APPLE__
		tmp=send(sock,start+sent,len-sent,SO_NOSIGPIPE);
#else		
		tmp=send(sock,start+sent,len-sent,MSG_NOSIGNAL);
#endif
		if(tmp == -1){
			if(errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			error("Error writing %i", errno);
			close(sock);
			cleanup_socket(sock);
			return -1;
		}

		sent+=tmp;
	}

	return sent;
}

void list_insert(socket_node** node, int s){
	socket_node** n=node;
	while(*n!=NULL)
		n=&(*n)->next;

	socket_node* new_node= malloc(sizeof(socket_node));

	(*n)=new_node;
	new_node->next=NULL;
	new_node->sock=s;

}

void list_delete(socket_node** node, int s){
	socket_node** n=node;
	socket_node* tmp;

	while(*n!=NULL){
		if((*n)->sock==s){
			if((*n)->player.z && (*n)->player.z->tiles[(*n)->player.x][(*n)->player.y].crtr == &((*n)->player)){
				(*n)->player.deceased=1;
				zone_rm_crtr((*n)->player.z, &((*n)->player));
			}

			tmp=*n;
			*n=(*n)->next;
			free(tmp);
			return;
		}

		n=&(*n)->next;
	}
}

void server_listen(socket_node* node){
	socket_node* n=node;
	list_altr=0;

	while(n != NULL){
	while(!read_packet(n->sock,n));

	//list status changed aborting
	if(list_altr)
		return;
	n=n->next;
	}

}

void server_tile_update(tile* t, int x, int y){
	socket_node* n=server_sockets;
	list_altr=0;

	while(n != NULL){
		write_tile_packet(n->sock,t,x,y);

		//if list is altered try again
		if(list_altr){
			server_tile_update(t,x,y);
			return;
		}

		n=n->next;
	}

}

void server_update_clients(){
	socket_node* n=server_sockets;
	zone* z;// = world.zones.arr[0];
	int x,y;

	while(n != NULL){
		z = n->player.z;
		write_player_packet(n->sock,&(n->player));
		write_time_packet(n->sock,&world.tm);

		if(list_altr)
			return;

		if(z)
			for(x=0;x<z->width;x++)
				for(y=0;y<z->height;y++)
					if((!z->tiles[x][y].impassible) && z->tiles[x][y].crtr!=&(n->player)){
						write_tile_packet(n->sock,&(z->tiles[x][y]),x,y);

						//list status changed aborting
						if(list_altr)
							return;
					}

		//attempts at reducing the number of tile updates
		/*for(i=0;i<z->crtrs.cnt;i++){
			x=((creature*)z->crtrs.arr[i])->x;
			y=((creature*)z->crtrs.arr[i])->y;
			write_tile_packet2(n->sock,&(z->tiles[x][y]),x,y);
		}*/

		n=n->next;
	}
}





