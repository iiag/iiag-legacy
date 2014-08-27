#include "packet.h"
#include "../commands.h"
#include "../player.h"
#include "../config.h"
#include "../inventory.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "../generator.h"

void write_spawn_packet(int sock){
	if(sock == -1) return;

	packet_header head;
	spawn_packet p;

	head.type=0;
	head.length=sizeof(spawn_packet);

	full_write(sock,&head,sizeof(head));
	full_write(sock,&p,head.length);

}

void write_command_packet(int sock, int c){
	if(sock == -1) return;

	packet_header head;
	command_packet p;

	head.type=1;
	head.length=sizeof(command_packet);
	p.c=c;

	full_write(sock,&head,sizeof(head));
	full_write(sock,&p,head.length);

}

void write_player_packet(int sock, creature* c){
	if(sock == -1) return;

	packet_header head;
	creature_subpacket* p;

	head.type=4;
	head.length=sizeof(creature_subpacket);
	p= make_crtr_subpacket(c);

	full_write(sock,&head,sizeof(head));
	full_write(sock,p,head.length);

	free(p);

}

void write_tile_packet2(int sock, tile* t, int x, int y){
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

creature_subpacket* make_crtr_subpacket(creature* c){
	creature_subpacket* ret;
	ret = malloc(sizeof(creature_subpacket));
	
	ret->level=c->level;
	ret->xp=c->xp;
	ret->x=c->x;
	ret->y=c->y;
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
	
	//wrlog("spawn recived!");

	for(x=0;x<z->width;x++)
	for(y=0;y<z->height;y++)
		write_tile_packet2(s->sock,&(z->tiles[x][y]),x,y);
	

}

void handle_command(socket_node* s, void* pack, int len){
	//execute(c,((command_packet*)pack)->c);
	int act = ((command_packet*)pack)->c;

	if(!s->player.act){
		if(act == CTRL_LEFT)	crtr_act_aa_move(&(s->player), -1, 0);
		if(act == CTRL_RIGHT)	crtr_act_aa_move(&(s->player), 1, 0);
		if(act == CTRL_UP)	crtr_act_aa_move(&(s->player), 0, -1);
		if(act == CTRL_DOWN)	crtr_act_aa_move(&(s->player), 0, 1);
		if(act == CTRL_ULEFT)	crtr_act_aa_move(&(s->player), -1, -1);
		if(act == CTRL_DLEFT)	crtr_act_aa_move(&(s->player), -1, 1);
		if(act == CTRL_URIGHT)	crtr_act_aa_move(&(s->player), 1, -1);
		if(act == CTRL_DRIGHT)	crtr_act_aa_move(&(s->player), 1, 1);
	}


}

void handle_player(socket_node* s, void* pack, int len){
	
	creature_subpacket* p= pack;

	if(p->health < 1){
	plyr_ev_death(NULL,"Network lag");
	}
	

	crtr_tele(&PLYR, p->x,p->y, world.zones.arr[0]);
	subpack2crtr(&PLYR,pack);
	zone_update(world.zones.arr[0], p->x,p->y);

}

void handle_tile2(socket_node* s, void* pack, int len){
	tile_packet* t= pack;

	zone* z = world.zones.arr[0];

	z->tiles[t->x][t->y].impassible=t->impassible;

	inv_clear(z->tiles[t->x][t->y].inv);

	if(z->tiles[t->x][t->y].crtr && !plyr_is_me(z->tiles[t->x][t->y].crtr)){
		z->tiles[t->x][t->y].crtr->deceased=1;
		zone_rm_crtr(z, z->tiles[t->x][t->y].crtr);
	}

	int i;
	item_subpacket* item_sub;
	creature_subpacket* crtr_sub;

	void* subpack=pack;
	subpack+=sizeof(tile_packet);

	for(i=0;i<t->itemnum;i++){
		item_sub = subpack;
		item* it = gen_item_from_id(world.gitems,1,item_sub->gen_id,item_sub->gen_mat_id);
	
		it->restore_health=item_sub->restore_health;
		it->restore_stamina=item_sub->restore_stamina;
		it->modify_attack=item_sub->modify_attack;
		it->modify_ac=item_sub->modify_ac;
		it->slot=item_sub->slot;
		it->gen_id=item_sub->gen_id;
		it->gen_mat_id=item_sub->gen_mat_id;

		inv_add(z->tiles[t->x][t->y].inv,it);
		subpack += sizeof(item_subpacket);
	}
	//wrlog("%i %p %p",t->crtr, pack+len, subpack);
	if(t->crtr){
		crtr_sub=subpack;

		creature* cr = gen_crtr_from_id(world.gcrtrs,1,crtr_sub->gen_id);

		subpack2crtr(cr,crtr_sub);
		cr->x=t->x;
		cr->y=t->y;

		crtr_tele(cr, t->x, t->y, z);
		
	}
	//wrlog("s");

	//if(t->impassible){
		z->tiles[t->x][t->y].ch=t->ch;
		z->tiles[t->x][t->y].show_ch=t->show_ch;
	//}else
		zone_update(z, t->x, t->y);

}

void (*packet_handlers[])(socket_node* s, void* pack, int len) = {
	handle_spawn,
	handle_command,
	NULL,
	handle_tile2,
	handle_player
};