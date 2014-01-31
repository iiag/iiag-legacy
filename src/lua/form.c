//
// lua/form.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "form.h"
#include "../util.h"
#include "../world.h"
#include "../form/crtr.h"
#include "../form/item.h"

const char * slot_names[] = {
	"left-hand",
	"right-hand",
	"head",
	"torso",
	"cape",
	"legs",
	"feet"
};

static chtype get_chtype(lua_State * lstate, const char * name, chtype x)
{
	lua_checkstack(lstate, 1);
	lua_getfield(lstate, 1, name);

	if (lua_isstring(lstate, -1)) {
		x = lua_tostring(lstate, -1)[0];
	}

	lua_pop(lstate, 1);
	return x;
}

static int get_int(lua_State * lstate, const char * name, int x)
{
	lua_checkstack(lstate, 1);
	lua_getfield(lstate, 1, name);

	if (lua_isnumber(lstate, -1)) {
		x = lua_tointeger(lstate, -1);
	}

	lua_pop(lstate, 1);
	return x;
}

static char * get_string(lua_State * lstate, const char * name, char * x)
{
	lua_checkstack(lstate, 1);
	lua_getfield(lstate, 1, name);

	if (lua_isstring(lstate, -1)) {
		free(x);
		x = copy_str(lua_tostring(lstate, -1));
	}

	lua_pop(lstate, 1);
	return x;
}

static int get_slot(lua_State * lstate, const char * name, int x)
{
	int i;
	const char * str;

	lua_checkstack(lstate, 1);
	lua_getfield(lstate, 1, name);

	if (lua_isstring(lstate, -1)) {
		str = lua_tostring(lstate, -1);

		for (i = 0; i < sizeof(slot_names) / sizeof(*slot_names); i++) {
			if (!strcmp(slot_names[i], str)) {
				x = i;
				break;
			}
		}
	}

	lua_pop(lstate, 1);
	return x;
}

static int get_bool(lua_State * lstate, const char * name, int x)
{
	lua_checkstack(lstate, 1);
	lua_getfield(lstate, 1, name);

	if (lua_isboolean(lstate, -1)) {
		x = lua_toboolean(lstate, -1);
	}

	lua_pop(lstate, 1);
	return x;
}

	
int lcf_cform(lua_State * lstate)
{
	cform * cf;

	assert(lua_istable(lstate, 1)); // TODO use actual error reporting here

	cf = cform_new(get_chtype(lstate, "char", '?'));
	cf->name = get_string(lstate, "name", cf->name);
	cf->max_health  = get_int(lstate, "max_health",  cf->max_health);
	cf->max_stamina = get_int(lstate, "max_stamina", cf->max_stamina);
	cf->def_attack  = get_int(lstate, "attack",      cf->def_attack);
	cf->def_ac      = get_int(lstate, "ac",          cf->def_ac);

	assure_world();
	vector_append(&world.cforms, cf);

	return 0;
}

int lcf_iform(lua_State * lstate)
{
	iform * f;

	assert(lua_istable(lstate, 1));

	f = iform_new(0, get_chtype(lstate, "char", '?'));
	f->name = get_string(lstate, "name", f->name);
	f->restore_health  = get_int(lstate, "restore_health",  f->restore_health);
	f->restore_stamina = get_int(lstate, "restore_stamina", f->restore_stamina);
	f->modify_attack   = get_int(lstate, "modify_attack",   f->modify_attack);
	f->modify_ac       = get_int(lstate, "modify_ac",       f->modify_ac);
	f->weight          = get_int(lstate, "weight",          f->weight);
	f->type = (f->type & ~ITEM_CONSUMABLE) | (get_bool(lstate, "consumable", f->type & ITEM_CONSUMABLE) << ITEM_CONSUMABLE_SFT);
	f->type = (f->type & ~ITEM_EQUIPABLE)  | (get_bool(lstate, "equipable",  f->type & ITEM_EQUIPABLE)  << ITEM_EQUIPABLE_SFT);
	f->slot = get_slot(lstate, "slot", f->slot);

	assure_world();
	vector_append(&world.iforms, f);

	return 0;
}
