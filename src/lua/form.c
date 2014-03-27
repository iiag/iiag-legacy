//
// lua/form.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include "form.h"
#include "../item.h"
#include "../util.h"
#include "../world.h"
#include "../creature.h"

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


GET_TEMPLATE_FRONT(int, trigger)
	if (lua_isfunction(lstate, -1)) {
		return luaL_ref(lstate, LUA_REGISTRYINDEX);
	}
GET_TEMPLATE_END

static void set_creature(lua_State * lstate, creature * cf)
{
	cf->generic_name = get_string(lstate, "name", cf->generic_name);
	cf->freq        = get_int(lstate, "freq",        cf->freq);
	cf->health      = get_int(lstate, "health",      cf->health);
	cf->stamina     = get_int(lstate, "stamina",     cf->stamina);
	cf->max_health  = get_int(lstate, "max_health",  cf->max_health);
	cf->max_stamina = get_int(lstate, "max_stamina", cf->max_stamina);
	cf->attack      = get_int(lstate, "attack",      cf->attack);
	cf->ac          = get_int(lstate, "ac",          cf->ac);
	cf->sight       = get_int(lstate, "sight",       cf->sight);
	cf->on_spawn.lua_block = get_trigger(lstate, "on_spawn", cf->on_spawn.lua_block);
	cf->on_death.lua_block = get_trigger(lstate, "on_death", cf->on_death.lua_block);
	cf->on_lvlup.lua_block = get_trigger(lstate, "on_lvlup", cf->on_lvlup.lua_block);
}

int lcf_creature(lua_State * lstate)
{
	creature * c;

	// TODO use actual error reporting here
	assert(lua_istable(lstate, 1));
	assure_world();

	c = crtr_new(get_chtype(lstate, "char", '?'));
	set_creature(lstate, c);
	vector_append(&world.cforms, c);
	world.max_cforms_freq += c->freq;

	return 0;
}

int lcf_player(lua_State * lstate)
{
	assert(lua_istable(lstate, 1));
	assure_world();

	set_creature(lstate, &world.plyr);
	world.plyr.ch = get_chtype(lstate, "char", world.plyr.ch);

	return 0;
}

int lcf_item(lua_State * lstate)
{
	item * it;

	assert(lua_istable(lstate, 1));

	it = item_new(0, get_chtype(lstate, "char", '?'));
	it->name = get_string(lstate, "name", it->name);
	it->freq            = get_int(lstate, "freq",            it->freq);
	it->restore_health  = get_int(lstate, "restore_health",  it->restore_health);
	it->restore_stamina = get_int(lstate, "restore_stamina", it->restore_stamina);
	it->modify_attack   = get_int(lstate, "modify_attack",   it->modify_attack);
	it->modify_ac       = get_int(lstate, "modify_ac",       it->modify_ac);
	it->weight          = get_int(lstate, "weight",          it->weight);
	it->type = (it->type & ~ITEM_CONSUMABLE) | (get_bool(lstate, "consumable", it->type & ITEM_CONSUMABLE) << ITEM_CONSUMABLE_SFT);
	it->type = (it->type & ~ITEM_EQUIPABLE)  | (get_bool(lstate, "equipable",  it->type & ITEM_EQUIPABLE)  << ITEM_EQUIPABLE_SFT);
	it->slot = get_slot(lstate, "slot", it->slot);

	assure_world();
	vector_append(&world.iforms, it);
	world.max_iforms_freq += it->freq;

	return 0;
}
