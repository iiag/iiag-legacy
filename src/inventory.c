//
// inventory.c
//

#include <assert.h>
#include <stdlib.h>
#include "object.h"
#include "inventory.h"

#define REALLOC_SIZE 8

inventory * inv_new(int max)
{
	inventory * inv = malloc(sizeof(inventory));
	inv->max_weight = max;
	inv->size = 0;
	inv->weight = 0;
	inv->objs = NULL;
	return inv;
}

void inv_free(inventory * inv)
{
	int i;

	for (i = 0; i < inv->size; i++) {
		if (inv->objs[i] != NULL && ~inv->objs[i]->flags & FL_NOFREE) {
			obj_free(inv->objs[i]);
		}
	}

	free(inv->objs);
	free(inv);
}

int inv_add(inventory * inv, object * o)
{
	int i;

	if (!inv_try(inv, o)) return INVALID;

	for (i = 0; i < inv->size; i++) {
		if (inv->objs[i] == NULL) {
			inv->objs[i] = o;
			return i;
		}
	}

	inv->objs = realloc(inv->objs, REALLOC_SIZE * sizeof(object *));
	inv->objs[inv->size] = o;

	for (i = 1; i < REALLOC_SIZE; i++) {
		inv->objs[i + inv->size] = NULL;
	}

	inv->size += REALLOC_SIZE;
	inv->weight += o->weight;
	return inv->size - REALLOC_SIZE;
}

int inv_try(inventory * inv, object * o)
{
	if (inv->max_weight == INFINITE) return 1;
	return o->weight + inv->weight <= inv->max_weight;
}

int inv_rm(inventory * inv, int i)
{
	if (inv->objs[i] == NULL) return 0;
	obj_free(inv->objs[i]);
	inv->objs[i] = NULL;
	return 1;
}

char inv_ind2ch(inventory * inv, int i)
{
	if (i < 26) return 'a' + i;
	if (i < 52) return 'A' + i - 26;
	if (i < 62) return '0' + i - 52;
	assert(0); // TODO
}

int  inv_ch2ind(inventory * inv, char c)
{
	if (c <= '9') return (int)c + 52 - '0';
	if (c <= 'Z') return (int)c + 26 - 'A';
	if (c <= 'z') return (int)c - 'a';
	assert(0); // TODO
}
