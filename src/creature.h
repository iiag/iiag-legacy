//
// creature.h
//

#ifndef CREATURE_H
#define CREATURE_H

#include "object.h"
#include "zone.h"

struct object;

typedef struct{
	char *ability;
	Object *obj;
	tiles **;
}cretr;

#endif
