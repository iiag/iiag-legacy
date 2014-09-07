//
// tile_object.h
//

#ifndef TILE_OBJECT_H
#define TILE_OBJECT_H

typedef struct tile_object tile_object;

#include "zone.h"

#define OBJECT_STAIR 0
#define OBJECT_DOOR  1

struct tile_object {

	int type;
	chtype ch;

	//teleporter stuff
	int link_x, link_y;
	zone * link_z;


};

tile_object* make_stair();
tile_object* make_door(bool open);

void use_stair(tile* t, int x, int y,creature* c, zone* z);
void use_door(tile* t, int x, int y,creature* c, zone* z);


#define OBJECT_USE_SIZE 2
void (*use_object[OBJECT_USE_SIZE])(tile* t, int x, int y,creature* c, zone* z);



#endif
