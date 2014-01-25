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
const char * iform_file = "script/iforms";
const char * cform_file = "script/cforms";

static char * type_names[] = {
	"useless",
	"consumable",
	"equipable",
};

static char * slot_names[] = {
	"left-hand",
	"right-hand",
	"head"
};

void load_iforms(void)
{
	iform * def = iform_new(USELESS, '?');
	iml_lang * lang = iml_lang_new(iform, def);

	iml_lang_add(lang, IML_INT,    "weight",          iform, weight);
	iml_lang_add(lang, IML_CHAR,   "char",            iform, ch);
	iml_lang_add(lang, IML_STRING, "name",            iform, name);
	iml_lang_add(lang, IML_INT,    "restore_health",  iform, u.cn.restore_health);
	iml_lang_add(lang, IML_INT,    "restore_stamina", iform, u.cn.restore_stamina);
	iml_lang_add(lang, IML_INT,    "modify_attack",   iform, u.eq.modify_attack);
	iml_lang_add(lang, IML_INT,    "modify_ac",       iform, u.eq.modify_ac);
	iml_lang_add_enum(lang, "type", iform, type, type_names);
	iml_lang_add_enum(lang, "slot", iform, u.eq.slot, slot_names);

	world.iforms = iml_read(iform_file, lang);

	world.iform_cnt = 0;
	while (world.iforms[world.iform_cnt] != NULL) {
		world.iform_cnt++;
	}

	iform_free(def);
}

void load_cforms(void)
{
	cform * def = cform_new('?');
	iml_lang * lang = iml_lang_new(cform, def);

	iml_lang_add(lang, IML_CHAR, "char", cform, ch);
	iml_lang_add(lang, IML_STRING, "name", cform, name);
	iml_lang_add(lang, IML_INT, "max_health", cform, max_health);
	iml_lang_add(lang, IML_INT, "max_stamina", cform, max_stamina);

	world.cforms = iml_read(cform_file, lang);

	world.cform_cnt = 0;
	while (world.cforms[world.cform_cnt] != NULL) {
		world.cform_cnt++;
	}

	cform_free(def);
}

void init_world(void)
{
	int x, y;
	cform * f;

	world.zones = malloc(sizeof(zone *));

	load_iforms();
	load_cforms();

	*world.zones = zone_new(get_dispw(), get_disph());

	f = cform_new('@' | A_BOLD);
	f->weight = TILE_MAX_WEIGHT / 2;
	f->max_health = 10;

	crtr_init(&world.plyr, f);
	world.plyr.nofree = 1;
	world.plyr.inv = inv_new(500);
	world.plyr.attack = 5;

	do {
		x = rand() % world.zones[0]->width;
		y = rand() % world.zones[0]->height;
	} while (!crtr_tele(&world.plyr, x, y, *world.zones));

	zone_update(world.plyr.z, x, y);
}
