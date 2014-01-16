//
// inventory.h
//

#ifndef INVENTORY_H
#define INVENTORY_H

#define INFINITE (-1)
#define INVALID  (-1)

struct object;

typedef struct inventory {
	int size;
	int weight;
	int max_weight;
	struct object ** objs;
} inventory;

inventory * inv_new(int);
void inv_free(inventory *);

int inv_add(inventory *, struct object *);
int inv_try(inventory *, struct object *);
int inv_rm(inventory *, int);
char inv_ind2ch(inventory *, int);
int  inv_ch2ind(inventory *, char);

#endif
