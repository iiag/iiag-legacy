//
// lua/form.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
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

// oh god, this is awful
#define GET_TEMPLATE_FRONT(TY, TYNM) \
static TY get_##TYNM(lua_State * lstate, const char * name, TY x) \
{ \
	lua_checkstack(lstate, 1); \
	lua_getfield(lstate, 1, name);

#define GET_TEMPLATE_END \
	lua_pop(lstate, 1); \
	return x; \
}


GET_TEMPLATE_FRONT(chtype, chtype)
	if (lua_isstring(lstate, -1)) {
		x = lua_tostring(lstate, -1)[0];
	}
GET_TEMPLATE_END


GET_TEMPLATE_FRONT(int, int)
	if (lua_isnumber(lstate, -1)) {
		x = lua_tointeger(lstate, -1);
	}
GET_TEMPLATE_END


GET_TEMPLATE_FRONT(char *, string)
	if (lua_isstring(lstate, -1)) {
		free(x);
		x = copy_str(lua_tostring(lstate, -1));
	}
GET_TEMPLATE_END


GET_TEMPLATE_FRONT(int, slot)
	int i;
	const char * str;

	if (lua_isstring(lstate, -1)) {
		str = lua_tostring(lstate, -1);

		for (i = 0; i < sizeof(slot_names) / sizeof(*slot_names); i++) {
			if (!strcmp(slot_names[i], str)) {
				x = i;
				break;
			}
		}
	}
GET_TEMPLATE_END


GET_TEMPLATE_FRONT(int, bool)
	if (lua_isboolean(lstate, -1)) {
		x = lua_toboolean(lstate, -1);
	}
GET_TEMPLATE_END


GET_TEMPLATE_FRONT(trigger, trigger)
	if (lua_isfunction(lstate, -1)) {
		return luaL_ref(lstate, LUA_REGISTRYINDEX);
	}
GET_TEMPLATE_END

static void set_cform(lua_State * lstate, cform * cf)
{
	cf->name = get_string(lstate, "name", cf->name);
	cf->freq        = get_int(lstate, "freq",        cf->freq);
	cf->max_health  = get_int(lstate, "max_health",  cf->max_health);
	cf->max_stamina = get_int(lstate, "max_stamina", cf->max_stamina);
	cf->def_attack  = get_int(lstate, "attack",      cf->def_attack);
	cf->def_ac      = get_int(lstate, "ac",          cf->def_ac);
	cf->on_spawn    = get_trigger(lstate, "on_spawn", cf->on_spawn);
}
	
int lcf_cform(lua_State * lstate)
{
	cform * cf;

	// TODO use actual error reporting here
	assert(lua_istable(lstate, 1));
	assure_world();

	cf = cform_new(get_chtype(lstate, "char", '?'));
	set_cform(lstate, cf);
	vector_append(&world.cforms, cf);
	world.max_cforms_freq += cf->freq;

	return 0;
}

int lcf_player(lua_State * lstate)
{
	assert(lua_istable(lstate, 1));
	assure_world();

	set_cform(lstate, world.plyr_form);
	world.plyr_form->ch = get_chtype(lstate, "char", world.plyr_form->ch);

	return 0;
}

int lcf_iform(lua_State * lstate)
{
	iform * f;

	assert(lua_istable(lstate, 1));

	f = iform_new(0, get_chtype(lstate, "char", '?'));
	f->name = get_string(lstate, "name", f->name);
	f->freq            = get_int(lstate, "freq",            f->freq);
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
	world.max_iforms_freq += f->freq;

	return 0;
}
