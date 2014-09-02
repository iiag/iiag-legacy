//
// generator.h
//

#ifndef GENERATOR_H
#define GENERATOR_H

#define MAX_LEVEL 10

typedef struct gelm gelm_t;
typedef struct gitem gitem_t;
typedef struct gclass gclass_t;

#include "util.h"
#include "item.h"
#include "creature.h"
#include "room.h"

struct gelm {
	int lvl;
	int freq, adj_freq;
	void * stuff;
};

struct gclass {
	char * name;
	int lvl_cached;
	int max_freq;
	vector_t elm;
	vector_t sub;
	gclass_t * par;
};

gclass_t * new_gclass(gclass_t *);
gclass_t * get_gclass(const char *, gclass_t *);
void       add_gelm(gclass_t *, int, int, void *);
item     * gen_item(gclass_t *, int);
creature * gen_crtr(gclass_t *, int);
room     * gen_room(gclass_t *, int);
item     * gen_item_from_id(gclass_t *, int, int, int);
creature * gen_crtr_from_id(gclass_t *, int, int);

#endif
