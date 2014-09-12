#include "tile_object.h"
#include "world.h"
#include "zone.h"
#include "player.h"
#include "io/display.h"
#include "net/packet.h"
#include "net/net.h"
#include "recipe.h"
#include "log.h"
#include "tileset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

const char * tile_obj_names[OBJ_NUM]={
"stair",
"door",
"smelter",
"forge",
"fire",
};

void (*use_object[OBJ_NUM])(tile* t, int x, int y,creature* c, zone* z) = {
	use_stair,
	use_door,
	use_craft,
	use_craft,
	use_craft,
};

tile_object* make_stair(){
	tile_object* ret;
	ret = malloc(sizeof(tile_object)); 

	ret->type=OBJECT_STAIR;
	ret->tile = TILE_STAIRS;
	ret->link_z = NULL;

	return ret;
}

tile_object* make_door(int open, int hdoor){
	tile_object* ret;
	ret = malloc(sizeof(tile_object)); 

	ret->type=OBJECT_DOOR;

	if (hdoor) {
		ret->tile = (open?TILE_HDOOR_OPEN:TILE_HDOOR_CLOSE);
	} else {
		ret->tile = (open?TILE_VDOOR_OPEN:TILE_VDOOR_CLOSE);
	}

	return ret;
}

tile_object* make_smelter(){
	tile_object* ret;
	ret = malloc(sizeof(tile_object)); 

	ret->type=OBJECT_SMELTER;
	ret->tile = TILE_SMELTER;

	return ret;
}

tile_object* make_forge(){
	tile_object* ret;
	ret = malloc(sizeof(tile_object)); 

	ret->type=OBJECT_FORGE;
	ret->tile = TILE_FORGE;

	return ret;
}

tile_object* make_fire(){
	tile_object* ret;
	ret = malloc(sizeof(tile_object)); 

	ret->type=OBJECT_FIRE;
	ret->tile = TILE_FIRE;

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

void use_door(tile* t, int x, int y,creature* c, zone* z)
{
	assert(t->obj);
	assert(t->obj->type == OBJECT_DOOR);

	z->tiles[x][y].impassible = !z->tiles[x][y].impassible;

	switch (t->obj->tile) {
	case TILE_VDOOR_OPEN:  t->obj->tile = TILE_VDOOR_CLOSE; break;
	case TILE_VDOOR_CLOSE: t->obj->tile = TILE_VDOOR_OPEN;  break;
	case TILE_HDOOR_OPEN:  t->obj->tile = TILE_HDOOR_CLOSE; break;
	case TILE_HDOOR_CLOSE: t->obj->tile = TILE_HDOOR_OPEN;  break;
	default:
		error("Trying to use a door that is not a door tile.");
		break;
	}
	zone_update(z,x,y);

#ifdef SERVER
	write_tile_packet(list_find_sock(c),t,x,y);
#else
	redraw();
#endif

}

void use_craft(tile* t, int x, int y,creature* c, zone* z)
{
	assert(t->obj);

	int k,i,j,f,f2,pos,dx,dy;

	dx = c->x-x;
	dy = c->y-y;
	pos = 3*(dy+1)+(dx+1)+1;

	for(k=0;k<recipes.cnt;k++){

		if(inv_count(t->inv) != ((recipe*)recipes.arr[k])->comps.cnt)
			continue;
		if(t->obj->type != ((recipe*)recipes.arr[k])->obj_type)
			continue;
		if(((recipe*)recipes.arr[k])->obj_arg && ((recipe*)recipes.arr[k])->obj_arg != pos)
			continue;

		f=1;
		for(i=0;i<((recipe*)recipes.arr[k])->comps.cnt;i++){
			f2=1;
			for(j=0;j<t->inv->size;j++)
				if(t->inv->itms[j])
					if(is_class(((component*)((recipe*)recipes.arr[k])->comps.arr[i])->iclass, t->inv->itms[j]->iclass))
						f2=0;
			if(f2){
				f=0;
				break;
			}
		}

		if(f){
			recipe_make(t->inv,recipes.arr[k]);
			info("smelting");/*make item and end*/
			return;
		}
	}	


	memo("Thou starest at the %s uselessly",tile_obj_names[t->obj->type]);

}


