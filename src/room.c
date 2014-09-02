#include "room.h"
#include "zone.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#define shape_square 0
#define shape_diamond 1
#define shape_circle 2
//more like rounded square....

void build_room(zone* z,room* r){

	int x,y;
	assert(r->max_height != r->min_height);
	assert(r->max_width != r->min_width);
	assert(z != NULL && r != NULL);

	r->w = random() % (r->max_width - r->min_width) + r->min_width;
	r->h = random() % (r->max_height - r->min_height) + r->min_height;
	r->x = random() % (z->width - (r-> w +3)) + 2;
	r->y = random() % (z->height - (r-> h +3)) + 2;

	if(r->shape == shape_square)
		for(x=0;x<r->w;x++)
		for(y=0;y<r->h;y++)
			zone_empty_tile(z,x+r->x,y+r->y);
		
			
	if(r->shape == shape_diamond){
		for(x=-(r->w/2);x<(r->w/2);x++)
		for(y=-(r->h/2);y<(r->h)/2;y++)
			if(abs(x)+abs(y)<(r->w/2))
			zone_empty_tile(z,x+r->x+(r->w/2),y+r->y+(r->h/2));
	}

	if(r->shape == shape_circle){
		for(x=-(r->w/2);x<(r->w/2);x++)
		for(y=-(r->h/2);y<(r->h/2);y++)
			if(x*x+y*y<(r->w*r->w/4))
				zone_empty_tile(z,x+r->x+(r->w/2),y+r->y+(r->h/2));
	}
	

}

void room_spot(zone* z,room* r, int* x, int* y){
	int timeout=0;
	do{
		*x= random() % r->w + r->x;
		*y= random() % r->h + r->y;
		timeout++;
		if(timeout>1000){
			*x=-1;
			*y=-1;
			break;
		}
	}while(z->tiles[*x][*y].impassible);
}
