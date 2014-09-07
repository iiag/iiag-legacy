#include "tile_object.h"
#include "world.h"
#include "zone.h"
#include "player.h"
#include "io/display.h"
#include "net/packet.h"
#include "net/net.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

tile_object* make_stair(){
	tile_object* ret;
	ret = malloc(sizeof(tile_object)); 

	ret->type=OBJECT_STAIR;
	ret->ch = '@';
	ret->link_z = NULL;

	return ret;
}

tile_object* make_door(bool open){
tile_object* ret;
	ret = malloc(sizeof(tile_object)); 

	ret->type=OBJECT_DOOR;
	ret->ch = ACS_DEGREE | (open?A_DIM:A_REVERSE);

	return ret;
}

void use_stair(tile* t, int ox, int oy,creature* c, zone* oz){

	assert(t->obj);
	assert(t->obj->type == OBJECT_STAIR);

	tile_object* obj, *new_obj;
	obj = t->obj;


	if (obj->link_z == NULL) {

		// generate new zone
		vector_append(&world.zones, zone_new(ZONE_SIZE_X, ZONE_SIZE_Y));
		obj->link_z = world.zones.arr[world.zones.cnt-1];

		// place player randomly
		crtr_spawn(c, obj->link_z);
		obj->link_x = c->x;
		obj->link_y = c->y;

			// link back
		t = tileof(c);
		if(t->obj)
			free(t->obj);
		new_obj = t->obj = make_stair();

		new_obj->link_x = ox;
		new_obj->link_y = oy;
		new_obj->link_z = oz;

	} else {
		if (!crtr_tele(c, obj->link_x, obj->link_y, obj->link_z)) {
			if(plyr_is_crtr(c))
			memo("Your way appears to be blocked?");
		}
	}

	zone_update(c->z,c->x,c->y);

	#ifndef SERVER
	if(plyr_is_crtr(c)){
		update_vis();
		scroll_center(c->x,c->y);
		zone_draw(c->z);
	}
	#else
	int x,y;
	if(plyr_is_crtr(c)){
		int sock = list_find_sock(c);
		list_altr=0;

		for(x=0;x<c->z->width;x++)
		for(y=0;y<c->z->height;y++)
			if(! (x == c->x && y == c->y) && (!list_altr))
				write_tile_packet(sock,&(c->z->tiles[x][y]),x,y);
		
		if(!list_altr)
		write_player_packet(sock,c);
		if(!list_altr)
		write_zone_packet(sock,c->z->name);
	}
	#endif
	
}

void use_door(tile* t, int x, int y,creature* c, zone* z){
	
	assert(t->obj);
	assert(t->obj->type == OBJECT_DOOR);

	z->tiles[x][y].impassible = !z->tiles[x][y].impassible;
	t->obj->ch = ACS_DEGREE | (z->tiles[x][y].impassible?A_REVERSE:A_DIM);
	zone_update(z,x,y);
	
	#ifdef SERVER
		write_tile_packet(list_find_sock(c),t,x,y);
	#else
		redraw();
	#endif

}

void (*use_object[OBJECT_USE_SIZE])(tile* t, int x, int y,creature* c, zone* z) = {
	use_stair,
	use_door,

};


