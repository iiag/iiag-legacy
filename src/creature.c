//
// creature.c
//

#include <stdlib.h>
#include "log.h"
#include "zone.h"
#include "world.h"
#include "display.h"
#include "creature.h"

creature * crtr_new(cform * f)
{
	creature * c = malloc(sizeof(creature));

	c->nofree = 0;
	c->f = form_copy(f);
	c->health = f->max_health;
	c->x = c->y = 0;
	c->z = NULL;
	c->ability = NULL;
	c->attack = 1;
	c->ac = 1;
	c->name = NULL;

	return c;
}

void crtr_free(creature * c)
{
	if (!c->nofree) {
		cform_free(c->f);
		free(c);
	}
}

static void update2(zone * z, int x1, int y1, int x2, int y2)
{
	if (z != NULL && z == world.plyr.z) {
		zone_update(z, x1, y1);
		zone_update(z, x2, y2);
		wrefresh(dispscr);
	}
}

int crtr_tele(creature * crtr, int x, int y, zone * z)
{
	tile * t = zone_at(z, x, y);

	if (t != NULL && t->crtr == NULL && !t->impassible) {
		z->tiles[x][y].crtr = crtr;

		if (crtr->z != NULL) {
			tileof(crtr)->crtr = NULL;
		}

		update2(crtr->z, x, y, crtr->x, crtr->y);

		crtr->x = x;
		crtr->y = y;
		crtr->z = z;

		return 1;
	}

	return 0;
}

int crtr_move(creature * crtr, int dx, int dy)
{
	int ox = crtr->x;
	int oy = crtr->y;
	int nx = ox + dx;
	int ny = oy + dy;

	return crtr_tele(crtr, nx, ny, crtr->z);
}

int crtr_attack(creature * attacker, creature * defender)
{
	int damage;

	damage = rand() % (attacker->attack + 1);
	damage -= rand() % (defender->ac + 1);
	if (damage < 0) damage = 0;

	defender->health -= damage;

	if (defender->health <= 0) return DEAD;
	return damage;
}
