//
// inventory.c
//

#include <assert.h>
#include <stdlib.h>
#include "display.h"
#include "inventory.h"

inventory * inv_new(int max)
{
	inventory * inv = malloc(sizeof(inventory));
	inv->max_weight = max;
	inv->size = 0;
	inv->weight = 0;
	inv->itms = NULL;
	return inv;
}

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

int inv_add(inventory * inv, item * it)
{
#define REALLOC_SIZE 8

	int i;

	if (!inv_try(inv, it)) return INVALID;
	inv->weight += it->f->weight;

	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] == NULL) {
			inv->itms[i] = it;
			return i;
		}
	}

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

int inv_try(inventory * inv, item * it)
{
	if (inv->max_weight == INFINITE) return 1;
	return it->f->weight + inv->weight <= inv->max_weight;
}

item * inv_rm(inventory * inv, int i)
{
	if (inv->itms[i] == NULL) return NULL;
	
	item * ret = inv->itms[i];
	inv->weight -= inv->itms[i]->f->weight;
	inv->itms[i] = NULL;

	return ret;
}

int inv_prompt(const char * prompt, inventory * inv)
{
	int i;
	
	wmove(dispscr, 0, 0);
	wprintw(dispscr, "%s\n", prompt);

	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] != NULL) {
			wprintw(dispscr, " %c) %s\n",
				ind2ch(i),
				inv->itms[i]->f->name
			);
		}
	}

	wrefresh(dispscr);
	return ch2ind(wgetch(dispscr));
}

char ind2ch(int i)
{
	if (i < 26) return 'a' + i;
	if (i < 52) return 'A' + i - 26;
	if (i < 62) return '0' + i - 52;
	assert(0); // TODO
}

int  ch2ind(char c)
{
	if (c <= '9') return (int)c + 52 - '0';
	if (c <= 'Z') return (int)c + 26 - 'A';
	if (c <= 'z') return (int)c - 'a';
	assert(0); // TODO
}
