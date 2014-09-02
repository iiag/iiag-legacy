#ifndef ROOM_H
#define ROOM_H
#include "zone.h"

typedef struct {//specifies room with x and y coordinates and w and h for width and height
	int max_width;
	int max_height;
	int min_width;
	int min_height;
	int shape;
	int features;

	int x, y, w, h;
} room;

//generate room
void build_room(zone* z,room* r);
void room_spot(zone* z,room* r, int* x, int* y);

#endif
