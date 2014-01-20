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
	c->step = 1;

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

void crtr_step(creature * c, int step)
{
	int dam;
	int dx = 0, dy = 0;

	if (c != &world.plyr && c->step != step) {
		// this is pretty ugly

		if (c->x > world.plyr.x) dx = -1;
		else if (c->x < world.plyr.x) dx = 1;

		if (c->y > world.plyr.y) dy = -1;
		else if (c->y < world.plyr.y) dy = 1;

		if (PLYR.x == c->x + dx && PLYR.y == c->y + dy) {
			dam = crtr_attack(c, &PLYR);

			if (dam) {
				memo("%s hits you for %d damage", c->f->name, dam);
			} else {
				memo("%s misses you", c->f->name);
			}
		} else {
			if (!crtr_move(c, dx, dy) && dx && dy) {
				if (!crtr_move(c, dx, 0)) {
					crtr_move(c, 0, dy);
				}
			}
		}

		c->step = step;
	}
}
