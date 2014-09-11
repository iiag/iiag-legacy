//
// tile_object.h
//

#ifndef TILE_OBJECT_H
#define TILE_OBJECT_H

typedef struct tile_object tile_object;

#include "zone.h"

#define OBJECT_STAIR 0
#define OBJECT_DOOR  1
#define OBJECT_SMELTER  2
#define OBJECT_FORGE  3
#define OBJECT_FIRE  4

#define OBJ_NUM 5
extern const char * tile_obj_names[OBJ_NUM];
void (*use_object[OBJ_NUM])(tile* t, int x, int y,creature* c, zone* z);

struct tile_object {

	int type;
	int tile;

	//teleporter stuff
	int link_x, link_y;
	zone * link_z;


};

tile_object* make_stair();
tile_object* make_door(int open);
tile_object* make_smelter();
tile_object* make_forge();
tile_object* make_fire();

void use_stair(tile* t, int x, int y,creature* c, zone* z);
void use_door(tile* t, int x, int y,creature* c, zone* z);
void use_craft(tile* t, int x, int y,creature* c, zone* z);







#endif
