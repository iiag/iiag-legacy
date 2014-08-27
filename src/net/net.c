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

int listening_socket = -1;
int client_socket = -1;
socket_node* server_sockets = NULL;

//socket creation should probably be moved
int client_connect(char* ip, int port){

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
		wrlog("new connection");
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
		wrlog("Fragmented header! %i Closing Connection",count);
		close(socket);
		cleanup_socket(socket);
		return 1;
	}
	void* packet= malloc(head.length);
	total=0;

	while(13){
		count= read(socket,packet+total,head.length-total);
		total+=count;

		if(total>=head.length)
			break;
	}
	packet_handlers[head.type](s,packet,head.length);
	free(packet);
	return 0;
}

int full_write(int sock, void* start, int len){
	int sent=0;
	int tmp;
	wrlog("sending");
	while(sent<len){
		tmp=send(sock,start+sent,len-sent,MSG_NOSIGNAL);

		if(tmp == -1){
			if(errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			wrlog("error encountered %i", errno);
			return -1;
		}

		sent+=tmp;
	}
	wrlog("sent");
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

	while(n != NULL){
	while(!read_packet(n->sock,n));
	
	n=n->next;
	}

}

void server_tile_update(tile* t, int x, int y){
	socket_node* n=server_sockets;

	while(n != NULL){
	write_tile_packet2(n->sock,t,x,y);
	
	n=n->next;
	}

}

void server_update_clients(){
	/*socket_node* n=server_sockets;

	while(n != NULL){
	if(n->player.act==NULL)
	crtr_act_idle(&(n->player));
	
	n=n->next;
	}*/

}





