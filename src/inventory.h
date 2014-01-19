//
// inventory.h
//

struct inventory;

#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"

#define INFINITE (-1)
#define INVALID  (-1)

typedef struct inventory {
	int size;
	int weight;
	int max_weight;
	struct item ** itms;
} inventory;

inventory * inv_new(int);
void inv_free(inventory *);

int inv_add(inventory *, struct item *);
int inv_try(inventory *, struct item *);
struct item * inv_rm(inventory *, int);
int inv_prompt(const char *, inventory *);

char ind2ch(int);
int  ch2ind(char);

#endif
