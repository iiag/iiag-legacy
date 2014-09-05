//
// inventory.c
//

#include <assert.h>
#include <stdlib.h>
#include "inventory.h"
#include "io/display.h"

//
// Allocates a new inventory
//
inventory * inv_new(int max)
{
	inventory * inv = malloc(sizeof(inventory));
	inv->max_weight = max;
	inv->size = 0;
	inv->weight = 0;
	inv->itms = NULL;
	return inv;
}

//
// Frees an inventory
//
void inv_free(inventory * inv)
{
	int i;

	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] != NULL) {
			item_free(inv->itms[i]);
		}
	}

	free(inv->itms);
	free(inv);
}

//
// Adds an item to an inventory
// Fails if inv_try would fail
//
int inv_add(inventory * inv, item * it)
{
#define REALLOC_SIZE 8

	int i;

	// See if item fits
	if (!inv_try(inv, it)) return INVALID;
	inv->weight += it->weight;

	// Look for free slot
	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] == NULL) {
			inv->itms[i] = it;
			return i;
		}
	}

	// No free slots, must add slots
	if (inv->size % REALLOC_SIZE == 0) {
		inv->itms = realloc(inv->itms,
				(inv->size + REALLOC_SIZE) * sizeof(item *)
		);
	}
	inv->itms[inv->size] = it;

	for (i = 1; i < REALLOC_SIZE; i++) {
		inv->itms[i + inv->size] = NULL;
	}

	inv->size += REALLOC_SIZE;
	return inv->size - REALLOC_SIZE;
}

//
// Fails if new weight exceeds max weight
//
int inv_try(inventory * inv, item * it)
{
	if (inv->max_weight == INFINITE) return 1;
	return it->weight + inv->weight <= inv->max_weight;
}

//
// Removes an item given an index
//
// Should we check for the index being outside the inventory
//
item * inv_rm(inventory * inv, int i)
{
	if (inv->itms[i] == NULL) return NULL;

	item * ret = inv->itms[i];
	inv->weight -= inv->itms[i]->weight;
	inv->itms[i] = NULL;

	return ret;
}

//
//
//
int inv_count(inventory* inv){
	int i;
	int count=0;
	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] != NULL)
			count++;
	}

return count;
}

//
//
//
void inv_clear(inventory * inv)
{
	int i;
	for (i = 0; i < inv->size; i++) {
	if (inv->itms[i] == NULL) continue;

	item * ret = inv->itms[i];
	inv->itms[i] = NULL;
	item_free(ret);
	}
	inv->weight = 0;
}

//
// Converts an index to a character
//
// TODO fix for indexs >= 62
//
char ind2ch(int i)
{
	if (i < 26) return 'a' + i;
	if (i < 52) return 'A' + i - 26;
	if (i < 62) return '0' + i - 52;
	assert(0); // TODO: Handle inventories with more than 62 items
}

//
// Converts an character to an index
//
// TODO fix for characters not [a-zA-Z0-9]
//
int  ch2ind(char c)
{
	if (c <= '9') return (int)c + 52 - '0';
	if (c <= 'Z') return (int)c + 26 - 'A';
	if (c <= 'z') return (int)c - 'a';
	assert(0); // TODO: Handle inventories with more than 62 items
}
