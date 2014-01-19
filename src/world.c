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
#include "form/form.h"

world_st world;
const char * form_file = "script/forms";

static char * type_names[] = {
	"useless",
};

void load_iforms(void)
{
	iform * def = iform_new(USELESS, '?');
	iml_lang * lang = iml_lang_new(iform, def);

	iml_lang_add(lang, IML_INT, "weight", iform, weight);
	iml_lang_add(lang, IML_CHAR, "char", iform, ch);
	iml_lang_add(lang, IML_STRING, "name", iform, name);
	iml_lang_add_enum(lang, "type", iform, type, type_names);

	world.iforms = iml_read(form_file, lang);

	world.iform_cnt = 0;
	while (world.iforms[world.iform_cnt] != NULL) {
		world.iform_cnt++;
	}

	iform_free(def);
}

void init_world(void)
{
	int x, y;

	world.zones = malloc(sizeof(zone *));
	load_iforms();

	*world.zones = zone_new(80, 25);
	
	world.plyr.f = cform_new('@' | A_BOLD);
	world.plyr.f->weight = TILE_MAX_WEIGHT / 2;
	world.plyr.f->max_health = 10;
	world.plyr.health = 10;
	world.plyr.nofree = 1;
	world.plyr.inv = inv_new(500);

	do {
		x = rand() % world.zones[0]->width;
		y = rand() % world.zones[0]->height;
	} while (!crtr_tele(&world.plyr, x, y, *world.zones));
}
