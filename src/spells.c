#include <string.h>
#include <stdlib.h>
#include "creature.h"
#include "spells.h"
#include "io/display.h"
#include "io/input.h"
#include "player.h"

static int find_creature(creature ** ret, zone * z, int x, int y, int dx, int dy, int range) {
	int timeout = 100;

	x += dx;
	y += dy;
	range--;
	while (x >= 0 && y >= 0 && x < z->width && y < z->height && !z->tiles[x][y].impassible && timeout) {
		if (!range) return 2;
		if (x < 0 || x >= z->width) break;
		if (y < 0 || y >= z->height) break;
		if (z->tiles[x][y].impassible) break;

		if (NULL != (*ret = z->tiles[x][y].crtr)) return 0;

		x += dx;
		y += dy;

		timeout--;
		range--;
	}

	*ret = NULL;

	return 1; // No creature found
}

// This probably shouldn't be here
static void redraw(void)
{
	disp_clear();
	zone_draw(PLYR.z);
	disp_refresh();
}

static void spell_xp(spell * sp, int xp)
{
	sp->xp += xp;
	if (sp->xp >= 5) {
		sp->level++;
		memo("You are getting better at casting %s", sp->name);
	} else if ((sp->xp < 0) && (sp->level > 1)) {
		sp->level--;
		memo("You are getting worse at casting %s", sp->name);
	}
}


// TODO: Example fire spell
void test_fire(spell * sp, creature * c, zone * z)
{
	creature * target;
	int ret;
	int dx, dy;

	if (!input_prompt_dir("Cast where?", &dx, &dy)) {
		memo("That is not a direction!");
		redraw();
		return;
	}
	redraw();
	ret = find_creature(&target, z, c->x, c->y, dx, dy, -1); // TODO: hook into spell's range

	if (ret == 1) {
		memo("Your spell hits the wall and fizzles!");
		return;
	} else if (ret == 2) {
		memo("You hit nothing");
		return;
	}

	target->health -= sp->level * 2;
	spell_xp(sp, 1);

	if (target->health <= 0) {
		crtr_death(target, "fireball explosion");
		memo("The %s has been killed with fire!\n", crtr_name(target));
	} else {
		memo("The fireball hits the %s\n", crtr_name(target));
	}
}

// TODO: Load in from config file or something here, remove crappy default information
spell * spell_new()
{
	// SAMPLE CODE
/*
	spell * sp = malloc(sizeof(spell));

	sp->caliber = 0;
	strcpy(sp->name = calloc(1,sizeof(char) * strlen("fireball")+1), "fireball");
	sp->effect = test_fire;
	sp->xp = 0;
	sp->level = 1;

	return sp;
*/
	return NULL;
}

spell * spell_upgrade(spell * one, spell * two)
{

	return NULL;
}


void spell_free(spell * sp)
{
	free(sp);
}

