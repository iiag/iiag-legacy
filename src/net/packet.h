#ifndef PACKET_H
#define PACKET_H
#include "../creature.h"
#include "../zone.h"
#include "../world.h"
#include "net.h"
#include "../item.h"

typedef struct packet_header packet_header;
typedef struct spawn_packet spawn_packet;
typedef struct command_packet command_packet;
typedef struct tile_packet tile_packet;
typedef struct creature_subpacket creature_subpacket;
typedef struct item_subpacket item_subpacket;

struct packet_header{
	short type;
	short length;
};

struct spawn_packet{
	//arbitrary data for now, class data later?
	char data[32];
};

extern int net_inv_prompt_data;
struct command_packet{
	int c;
	int i;
};

struct item_subpacket{

	// consumable-specific data
	int restore_health;
	int restore_stamina;

	// equipable-specific data
	int modify_attack;
	int modify_ac;
	int slot;
	
	//id for serialization
	int gen_id;
	int gen_mat_id;
};

struct creature_subpacket{

	//id for serialization
	int gen_id;
	int x,y;
	int ai;

	// item fields
	int item_num;
	//inventory * inv;
	//item * slots[MAX_SLOTS];
	int slots[MAX_SLOTS];

	// level/xp
	int level;
	int xp, need_xp;

	// skills and stats
	int health, stamina;
	int max_health, max_stamina;
	int attack;
	int ac;
	int sight;
	int reflex;
	int throw;
	int speed;

};

struct tile_packet{
	chtype ch,show_ch;
	int itemnum;
	short crtr;
	int impassible;
	int x,y;
};

creature_subpacket* make_crtr_subpacket(creature* c);
item_subpacket* make_item_subpacket(item* c);

void write_spawn_packet(int sock);
void write_command_packet(int sock, int c);
void write_tile_packet(int sock, tile* t, int x, int y);
void write_player_packet(int sock, creature* c);
void write_time_packet(int sock, world_time_t* t);

void handle_spawn(socket_node* s,void* pack, int len);
void handle_command(socket_node* s,void* pack, int len);
void handle_tile(socket_node* s,void* pack, int len);
void handle_player(socket_node* s,void* pack, int len);
void handle_time(socket_node* s,void* pack, int len);

#define PACKET_HANDLERS_SIZE 5
void (*packet_handlers[PACKET_HANDLERS_SIZE])(socket_node* s, void* pack, int len);

#endif
