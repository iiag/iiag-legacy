//
// creature.h
//

#ifndef CREATURE_H
#define CREATURE_H

#include "object.h"
#include "zone.h"

struct object;

typedef struct creature{
	char *ability;
	struct object *obj;
}creature;

#endif
