//
// item.c
//

#include <stdlib.h>
#include "item.h"

item * item_new(iform * f)
{
	item * it = malloc(sizeof(item));
	it->of = NULL;
	it->i = 0;
	it->f = f;
	return it;
}

void item_free(item * it)
{
	free(it);
}

int item_tele(item * it, int x, int y, zone * z)
{
	int i = inv_add(z->tiles[x][y].inv, it);

	if (i != INVALID) {
		if (it->of != NULL) {
			inv_rm(it->of, it->i);
		}

		it->of = z->tiles[x][y].inv;
		it->i = i;

		return 1;
	}

	return 0;
}
