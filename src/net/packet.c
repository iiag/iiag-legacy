#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "packet.h"
#include "../commands.h"
#include "../player.h"
#include "../config.h"
#include "../inventory.h"
#include "../generator.h"
#include "../io/display.h"

int net_inv_prompt_data =-1;
short net_dir_prompt =-1;

short encode_dir(int x,int y){
return ((x+2)&0xFF)+(((y+2)&0xFF)<<8);
}
void  decode_dir(short d,int*x,int*y){
*x=(d&0xFF)-2;
*y=((d&0xFF00)>>8)-2;
}

void write_spawn_packet(int sock){
	if(sock == -1) return;

	packet_header head;
	spawn_packet p;

	head.type=0;
	head.length=sizeof(spawn_packet);

	full_write(sock,&head,sizeof(head));
	full_write(sock,&p,head.length);

}

void write_zone_packet(int sock, char * c){
	if(sock == -1) return;

	packet_header head;

	head.type=5;
	head.length=strlen(c)+1;
	full_write(sock,&head,sizeof(head));
	full_write(sock,c,head.length);
}


void write_memo_packet(int sock, char * c){
	if(sock == -1) return;

	packet_header head;

	head.type=6;
	head.length=strlen(c)+1;

	full_write(sock,&head,sizeof(head));
	full_write(sock,c,head.length);
}

void write_time_packet(int sock, world_time_t* t){
	if(sock == -1) return;

	packet_header head;

	head.type=2;
	head.length=sizeof(world_time_t);

	full_write(sock,&head,sizeof(head));
	full_write(sock,t,head.length);

}

void write_command_packet(int sock, int c){
	if(sock == -1) return;

	packet_header head;
	command_packet p;

	head.type=1;
	head.length=sizeof(command_packet);
	p.c=c;
	p.i=net_inv_prompt_data;
	p.d=net_dir_prompt;

	net_inv_prompt_data =-1;
	net_dir_prompt = -1;

	full_write(sock,&head,sizeof(head));
	full_write(sock,&p,head.length);

}

void write_player_packet(int sock, creature* c){
	if(sock == -1) return;

	int i,tmp;
	packet_header head;
	creature_subpacket* p;

	p= make_crtr_subpacket(c);
	p->item_num=inv_count(c->inv);

	for(i=0;i< MAX_SLOTS; i++)
		if(c->slots[i])
			p->slots[i]=c->slots[i]->i;
		else
			p->slots[i]=-1;

	head.type=4;
	head.length=sizeof(creature_subpacket)+p->item_num*sizeof(item_subpacket);


	tmp=0;
	full_write(sock,&head,sizeof(head));
	tmp+=full_write(sock,p,sizeof(creature_subpacket));

	item_subpacket* item_sub;
	for (i = 0; i < c->inv->size; i++) {
		if (c->inv->itms[i] != NULL){
			item_sub=make_item_subpacket(c->inv->itms[i]);
			tmp+=full_write(sock,item_sub,sizeof(item_subpacket));
			free(item_sub);
		}
	}
	free(p);

}

void write_tile_packet(int sock, tile* t, int x, int y){
	if(sock == -1) return;

	int i;

	packet_header head;
	tile_packet p;

	p.ch=t->ch;
	p.show_ch=t->show_ch;
	p.itemnum=inv_count(t->inv);
	p.crtr=(t->crtr?1:0);
	p.impassible=t->impassible;
	p.x=x;
	p.y=y;
	p.object_type=(t->obj?t->obj->type:-1);
	head.type=3;
	head.length=sizeof(tile_packet)+p.itemnum*sizeof(item_subpacket)+p.crtr*sizeof(creature_subpacket);

	full_write(sock,&head,sizeof(head));
	full_write(sock,&p,sizeof(tile_packet));

	item_subpacket* item_sub;
	creature_subpacket* crtr_sub;
	for (i = 0; i < t->inv->size; i++) {
		if (t->inv->itms[i] != NULL){
			item_sub=make_item_subpacket(t->inv->itms[i]);
			full_write(sock,item_sub,sizeof(item_subpacket));
			free(item_sub);
		}
	}

	if(p.crtr){
		crtr_sub= make_crtr_subpacket(t->crtr);
		full_write(sock,crtr_sub,sizeof(creature_subpacket));
		free(crtr_sub);
	}
}

item_subpacket* make_item_subpacket(item* c){
	item_subpacket* ret;
	ret = malloc(sizeof(item_subpacket));

	ret->restore_health=c->restore_health;
	ret->restore_stamina=c->restore_stamina;
	ret->modify_attack=c->modify_attack;
	ret->modify_ac=c->modify_ac;
	ret->slot=c->slot;
	ret->gen_id=c->gen_id;
	ret->gen_mat_id=c->gen_mat_id;

	return ret;
}

void subpack2item(item* it, item_subpacket* item_sub){

	it->restore_health=item_sub->restore_health;
	it->restore_stamina=item_sub->restore_stamina;
	it->modify_attack=item_sub->modify_attack;
	it->modify_ac=item_sub->modify_ac;
	it->slot=item_sub->slot;
	it->gen_id=item_sub->gen_id;
	it->gen_mat_id=item_sub->gen_mat_id;
}

creature_subpacket* make_crtr_subpacket(creature* c){
	creature_subpacket* ret;
	ret = malloc(sizeof(creature_subpacket));

	ret->level=c->level;
	ret->xp=c->xp;
	ret->x=c->x;
	ret->y=c->y;
	ret->ai=c->ai;
	ret->need_xp=c->need_xp;
	ret->health=c->health;
	ret->stamina=c->stamina;
	ret->max_health=c->max_health;
	ret->max_stamina=c->max_stamina;
	ret->attack=c->attack;
	ret->ac=c->ac;
	ret->sight=c->sight;
	ret->reflex=c->reflex;
	ret->throw=c->throw;
	ret->speed=c->speed;
	ret->gen_id=c->gen_id;

	return ret;
}

void subpack2crtr(creature* cr, creature_subpacket* crtr_sub){

	cr->level=crtr_sub->level;
	cr->x=crtr_sub->x;
	cr->y=crtr_sub->y;
	cr->xp=crtr_sub->xp;
	cr->ai=crtr_sub->ai;
	cr->need_xp=crtr_sub->need_xp;
	cr->health=crtr_sub->health;
	cr->stamina=crtr_sub->stamina;
	cr->max_health=crtr_sub->max_health;
	cr->max_stamina=crtr_sub->max_stamina;
	cr->attack=crtr_sub->attack;
	cr->ac=crtr_sub->ac;
	cr->sight=crtr_sub->sight;
	cr->reflex=crtr_sub->reflex;
	cr->throw=crtr_sub->throw;
	cr->speed=crtr_sub->speed;
	cr->gen_id=crtr_sub->gen_id;
}

void handle_spawn(socket_node* s, void* pack, int len){
	int x,y;

	crtr_init(&(s->player), '@' | A_BOLD);
	//s->player.on_death.c_func    = (trigger_cfunc)plyr_ev_death;
	s->player.on_lvlup.c_func    = (trigger_cfunc)plyr_ev_lvlup;
	//s->player.on_act_comp.c_func = (trigger_cfunc)plyr_ev_act_comp;
	//s->player.on_act_fail.c_func = (trigger_cfunc)plyr_ev_act_fail;
	s->player.refs = NOFREE;
	s->player.ai = 0;

	zone* z = world.zones.arr[0];
	crtr_spawn(&(s->player), z);
	zone_update(z, s->player.x, s->player.y);

	for(x=0;x<z->width;x++)
	for(y=0;y<z->height;y++)
		if(! (x == s->player.x && y == s->player.y) )
			write_tile_packet(s->sock,&(z->tiles[x][y]),x,y);

	write_player_packet(s->sock,&s->player);
	write_zone_packet(s->sock,z->name);
}

void handle_command(socket_node* s, void* pack, int len){
	int act = ((command_packet*)pack)->c;
	int i = ((command_packet*)pack)->i;
	int d = ((command_packet*)pack)->d;

	if(!s->player.act){
		if(act == CTRL_LEFT)	crtr_act_aa_move(&(s->player), -1, 0);
		if(act == CTRL_RIGHT)	crtr_act_aa_move(&(s->player), 1, 0);
		if(act == CTRL_UP)	crtr_act_aa_move(&(s->player), 0, -1);
		if(act == CTRL_DOWN)	crtr_act_aa_move(&(s->player), 0, 1);
		if(act == CTRL_ULEFT)	crtr_act_aa_move(&(s->player), -1, -1);
		if(act == CTRL_DLEFT)	crtr_act_aa_move(&(s->player), -1, 1);
		if(act == CTRL_URIGHT)	crtr_act_aa_move(&(s->player), 1, -1);
		if(act == CTRL_DRIGHT)	crtr_act_aa_move(&(s->player), 1, 1);

		if(i != -1){
			if(act == CTRL_DROP)	crtr_act_drop(&(s->player), i);
			if(act == CTRL_CONSUME)	crtr_act_consume(&(s->player), i);
			if(act == CTRL_PICKUP)	crtr_act_pickup(&(s->player), i);
			if(act == CTRL_EQUIP)	crtr_act_equip(&(s->player), i);
		}
		if(d != -1){
			int x,y;
			decode_dir(d,&x,&y);
			if(abs(x)+abs(y)>2){
				warning("dir %i %i is not valid!",x,y);
				return;
			}
			if(act == CTRL_USE)	crtr_act_use(&(s->player), x, y);
			if( i != -1)
				if(act == CTRL_THROW)	crtr_act_throw(&(s->player), i, x, y);
		}
	}
}

void handle_player(socket_node* s, void* pack, int len){

	void* subpack=pack+sizeof(creature_subpacket);
	creature_subpacket* p= pack;
	item_subpacket* item_sub;
	int i;

	if(p->health < 1){
		plyr_ev_death(NULL,"Network lag");
	}
	if(p->stamina <= 0){
		plyr_ev_death(NULL,"starvation");
	}

	crtr_tele(&PLYR, p->x,p->y, world.zones.arr[0]);
	subpack2crtr(&PLYR,p);
	zone_update(world.zones.arr[0], p->x,p->y);
	update_vis();

	inv_clear(PLYR.inv);

	for(i=0;i<p->item_num;i++){
		item_sub = subpack;
		item* it = gen_item_from_id(world.gitems,1,item_sub->gen_id,item_sub->gen_mat_id);

		subpack2item(it,item_sub);

		inv_add(PLYR.inv,it);
		subpack += sizeof(item_subpacket);
	}

	for(i=0;i< MAX_SLOTS; i++)
		if(p->slots[i] != -1)
			PLYR.slots[i]=PLYR.inv->itms[p->slots[i]];
		else
			PLYR.slots[i]=NULL;

}

void handle_tile(socket_node* s, void* pack, int len){
	tile_packet* t= pack;

	zone* z = world.zones.arr[0];

	z->tiles[t->x][t->y].impassible=t->impassible;
	if(t->impassible)
		z->tiles[t->x][t->y].show=0;

	//clear tile objects and load new one
	if(z->tiles[t->x][t->y].obj){ 
		free(z->tiles[t->x][t->y].obj);
		z->tiles[t->x][t->y].obj= NULL;
	}

	if(t->object_type == OBJECT_STAIR)
	z->tiles[t->x][t->y].obj = make_stair();
	if(t->object_type == OBJECT_DOOR)
	z->tiles[t->x][t->y].obj = make_door(!t->impassible);


	//clear inventory and creatures
	inv_clear(z->tiles[t->x][t->y].inv);

	if(z->tiles[t->x][t->y].crtr && !plyr_is_crtr(z->tiles[t->x][t->y].crtr)){
		z->tiles[t->x][t->y].crtr->deceased=1;
		zone_rm_crtr(z, z->tiles[t->x][t->y].crtr);
	}

	//read items
	int i;
	item_subpacket* item_sub;
	creature_subpacket* crtr_sub;

	void* subpack=pack;
	subpack+=sizeof(tile_packet);

	for(i=0;i<t->itemnum;i++){
		item_sub = subpack;
		item* it = gen_item_from_id(world.gitems,1,item_sub->gen_id,item_sub->gen_mat_id);

		subpack2item(it,item_sub);

		inv_add(z->tiles[t->x][t->y].inv,it);
		subpack += sizeof(item_subpacket);
	}
	
	//read creature
	if(t->crtr){
		crtr_sub=subpack;

		creature* cr = gen_crtr_from_id(world.gcrtrs,1,crtr_sub->gen_id);

		subpack2crtr(cr,crtr_sub);
		cr->x=t->x;
		cr->y=t->y;
		//creature is annother player!
		if(cr->ai == 0)
			cr->ch = ('@' | COLOR_PAIR(COLOR_OTHER));

		crtr_tele(cr, t->x, t->y, z);
	}


		z->tiles[t->x][t->y].ch=t->ch;
		z->tiles[t->x][t->y].show_ch=t->show_ch;
		zone_update(z, t->x, t->y);

}

void handle_time(socket_node* s,void* pack, int len){

memcpy(&world.tm,pack,sizeof(world_time_t));

}

void handle_zone(socket_node* s,void* pack, int len){

	zone* z = world.zones.arr[0];
	if(z->name) free(z->name);
	z->name = malloc(len);
	//better soulution would be a strcpy that specifies length
	memcpy(z->name,pack,len);


	zone_update(z,PLYR.x,PLYR.y);
	scroll_center(PLYR.x,PLYR.y);

	int x,y;
	for(x=0;x<z->width;x++)
	for(y=0;y<z->height;y++)
		if (z->tiles[x][y].impassible) {
			set_wall_char(z,x,y);
		}

	update_vis();
	scroll_view_center(0,NULL);
}

void handle_memo(socket_node* s,void* pack, int len){
	memo("%s",pack);
}

void (*packet_handlers[PACKET_HANDLERS_SIZE])(socket_node* s, void* pack, int len) = {
	handle_spawn,
	handle_command,
	handle_time,
	handle_tile,
	handle_player,
	handle_zone,
	handle_memo,
};
