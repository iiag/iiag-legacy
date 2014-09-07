#include "packet.h"

void write_test_packet(int sock){
packet_header head;
test_packet p;

head.type=0;
head.length=sizeof(test_packet);

write(sock,&head,sizeof(head));
write(sock,&p,head.length);

}

void handle_test(void* pack, int len){
printf("test!");
}


void (*packet_handlers[])(void* pack, int len) = {
handle_test
};
