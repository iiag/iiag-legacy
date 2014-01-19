//
// world.c
//

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "log.h"
#include "util.h"
#include "world.h"
#include "display.h"
#include "inventory.h"
#include "iml/iml.h"

world_st world;
const char * form_file = "script/forms";

static char * type_names[] = {
	"useless",
	"creature",
};

void load_forms(void)
{
	form * def = form_new(USELESS, '?');
	iml_lang * lang = iml_lang_new(form, def);

	iml_lang_add(lang, IML_INT, "weight", form, weight);
	iml_lang_add(lang, IML_CHAR, "char", form, ch);
	iml_lang_add(lang, IML_STRING, "name", form, name);
	iml_lang_add_enum(lang, "type", form, type, type_names);

	world.forms = iml_read(form_file, lang);

	world.form_count = 0;
	while (world.forms[world.form_count] != NULL) {
		world.form_count++;
	}

	form_free(def);
}

void init_world(void)
{
	int x, y;

	world.zones = malloc(sizeof(zone *));
	load_forms();

	*world.zones = zone_new(80, 25);
	world.plyr.obj = obj_new(form_new(CREATURE, '@' | A_BOLD));
	world.plyr.obj->f->weight = TILE_MAX_WEIGHT / 2;
	world.plyr.obj->f->max_health = 10;
	world.plyr.obj->health = 10;
	world.plyr.obj->flags |= FL_NOFREE;

	do {
		x = rand() % world.zones[0]->width;
		y = rand() % world.zones[0]->height;
	} while (!crtr_tele(&world.plyr, x, y, *world.zones));
}
