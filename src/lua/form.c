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
#include "../faction.h"
#include "../creature.h"
#include "../tileset.h"
#include "../room.h"
#include "../recipe.h"
#include "../tile_object.h"
#include "../log.h"

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

GET_TEMPLATE_FRONT(int, int)
	if (lua_isnumber(lstate, -1)) {
		x = lua_tointeger(lstate, -1);
	}
GET_TEMPLATE_END

/*
GET_TEMPLATE_FRONT(float, float)
	if (lua_isnumber(lstate, -1)) {
		x = lua_tonumber(lstate, -1);
	}
GET_TEMPLATE_END
*/

GET_TEMPLATE_FRONT(char *, string)
	if (lua_isstring(lstate, -1)) {
		free(x);
		x = copy_str(lua_tostring(lstate, -1));
	}
GET_TEMPLATE_END


GET_TEMPLATE_FRONT(const char *, cstring)
	if (lua_isstring(lstate, -1)) {
		x = lua_tostring(lstate, -1);
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


GET_TEMPLATE_FRONT(faction *, faction)
	if (lua_isstring(lstate, -1)) {
		x = fctn_get(lua_tostring(lstate, -1));
	}
GET_TEMPLATE_END

static void set_creature(lua_State * lstate, creature * cf)
{
	cf->generic_name = get_string(lstate, "name", cf->generic_name);
	cf->health      = get_int(lstate, "health",      cf->health);
	cf->stamina     = get_int(lstate, "stamina",     cf->stamina);
	cf->max_health  = get_int(lstate, "max_health",  cf->max_health);
	cf->max_stamina = get_int(lstate, "max_stamina", cf->max_stamina);
	cf->attack      = get_int(lstate, "attack",      cf->attack);
	cf->ac          = get_int(lstate, "ac",          cf->ac);
	cf->sight       = get_int(lstate, "sight",       cf->sight);
	cf->speed       = get_int(lstate, "speed",       cf->speed);
	cf->fctn        = get_faction(lstate, "faction", cf->fctn);
	cf->on_spawn.lua_block = get_trigger(lstate, "on_spawn", cf->on_spawn.lua_block);
	cf->on_death.lua_block = get_trigger(lstate, "on_death", cf->on_death.lua_block);
	cf->on_lvlup.lua_block = get_trigger(lstate, "on_lvlup", cf->on_lvlup.lua_block);
}

static void add_to_gclass(lua_State * lstate, gclass_t * root, void * stf)
{
	int freq, lvl;
	const char * cl_name;
	gclass_t * cl;

	freq = get_int(lstate, "freq",  0);
	lvl  = get_int(lstate, "level", 1);
	cl_name = get_cstring(lstate, "class", "");
	cl = get_gclass(cl_name, root);

	add_gelm(cl, freq, lvl, stf);
}

int lcf_creature(lua_State * lstate)
{
	creature * c;

	// TODO use actual error reporting here
	assert(lua_istable(lstate, 1));
	assure_world();

	c = crtr_new(get_int(lstate, "tile", TILE_UNKNOWN));
	set_creature(lstate, c);

	assure_world();
	add_to_gclass(lstate, world.gcrtrs, c);

	return 0;
}

int lcf_player(lua_State * lstate)
{
	assert(lua_istable(lstate, 1));
	assure_world();

	set_creature(lstate, &world.plyr);
	world.plyr.tile = get_int(lstate, "tile", world.plyr.tile);

	return 0;
}

int lcf_item(lua_State * lstate)
{
	item * it;

	assert(lua_istable(lstate, 1));

	it = item_new(0, get_int(lstate, "tile", TILE_UNKNOWN));

	//it->name = get_string(lstate, "name", it->name);
	//it->mat_class = get_string(lstate, "material", it->mat_class);

	char* tmp;
	tmp = get_string(lstate, "iclass", NULL);
	if(tmp){
		it->iclass=find_class(&item_types,tmp);
		info("item name %i %s",it->iclass,tmp);
		free(tmp);	
	}

	tmp = get_string(lstate, "material", NULL);
	if(tmp){
		it->mat=find_str(&materials,tmp);
		if(it->mat == -1){
			vector_append(&materials,tmp);
			it->mat = materials.cnt-1;
		}else
		free(tmp);	
	}

	it->restore_health  = get_int(lstate, "restore_health",  it->restore_health);
	it->restore_stamina = get_int(lstate, "restore_stamina", it->restore_stamina);
	it->modify_attack   = get_int(lstate, "modify_attack",   it->modify_attack);
	it->modify_ac       = get_int(lstate, "modify_ac",       it->modify_ac);
	it->weight          = get_int(lstate, "weight",          it->weight);
	it->spikiness       = get_int(lstate, "spikiness",       it->spikiness);
	it->type = (it->type & ~ITEM_CONSUMABLE) | (get_bool(lstate, "consumable", it->type & ITEM_CONSUMABLE) << ITEM_CONSUMABLE_SFT);
	it->type = (it->type & ~ITEM_EQUIPABLE)  | (get_bool(lstate, "equipable",  it->type & ITEM_EQUIPABLE)  << ITEM_EQUIPABLE_SFT);
	it->slot = get_slot(lstate, "slot", it->slot);

	it->quality = rand()%QUALITY_NUM;
	item_gen_name(it);

	assure_world();
	add_to_gclass(lstate, world.gitems, it);

	return 0;
}

int lcf_i_type(lua_State * lstate)
{
	assert(lua_istable(lstate, 1));

	char *tmp, *sub;
	i_class* type;

	tmp = get_string(lstate, "class", NULL);
	sub = get_string(lstate, "type_of", NULL);

	if(tmp != NULL){

		if(find_class(&item_types,tmp) == -1){

			type=malloc(sizeof(i_class));
			type->name=tmp;
			if(sub != NULL){
				type->type_of=find_class(&item_types,sub);
			}else{
				type->type_of=-1;
			}
			vector_append(&item_types,type);
		}else{
			free(tmp);
		}
	}
	free(sub);
	return 0;
}

int lcf_begin_recipe(lua_State * lstate)
{
	recipe * res;

	assert(lua_istable(lstate, 1));

	res = malloc(sizeof(recipe));
	vector_init(&res->comps);

	int x;
	const char * tmp;

	tmp = get_cstring(lstate, "class", NULL);

	if(tmp == NULL)
		return 0;

	res->iclass = find_class(&item_types,tmp);
	
	tmp = get_cstring(lstate, "made_by", NULL);
	res->obj_type=-1;
	if(tmp)
		for(x=0;x<OBJ_NUM;x++)
			if(!strcmp(tmp,tile_obj_names[x]))
				res->obj_type=x;
				
	res->obj_arg = get_int(lstate, "made_with",  0);

	tmp = get_cstring(lstate, "mat", NULL);
	if(tmp)
		res->mat=find_str(&materials,tmp);
	else
		res->mat=-1;

	//base stats
	res->restore_health  = get_int(lstate, "restore_health",  0);
	res->restore_stamina = get_int(lstate, "restore_stamina", 0);
	res->modify_attack   = get_int(lstate, "modify_attack",   0);
	res->modify_ac       = get_int(lstate, "modify_ac",       0);
	res->weight          = get_int(lstate, "weight",          0);
	res->spikiness       = get_int(lstate, "spikiness",       0);
	res->durability       = get_int(lstate, "durability",     0);
	
	res->attrb =0;
	res->attrb = (res->attrb & ~ITEM_CONSUMABLE) | (get_bool(lstate, "consumable", res->attrb & ITEM_CONSUMABLE) << ITEM_CONSUMABLE_SFT);
	res->attrb = (res->attrb & ~ITEM_EQUIPABLE)  | (get_bool(lstate, "equipable",  res->attrb & ITEM_EQUIPABLE)  << ITEM_EQUIPABLE_SFT);
	res->slot = get_slot(lstate, "slot", 0);
	res->ch = get_int(lstate, "tile", TILE_UNKNOWN);


	vector_append(&recipes,res);

	return 0;
}

int lcf_component(lua_State * lstate)
{
	component * com;

	assert(lua_istable(lstate, 1));

	com = malloc(sizeof(component));

	char * tmp;

	tmp = get_string(lstate, "class", NULL);

	if(tmp == NULL)
		return 0;

	com->iclass = find_class(&item_types,tmp);
	
	com->mat = get_string(lstate, "mat", NULL);

	//base stats
	com->restore_health  = get_string(lstate, "restore_health",  NULL);
	com->restore_stamina = get_string(lstate, "restore_stamina", NULL);
	com->modify_attack   = get_string(lstate, "modify_attack",   NULL);
	com->modify_ac       = get_string(lstate, "modify_ac",       NULL);
	com->weight          = get_string(lstate, "weight",          NULL);
	com->spikiness       = get_string(lstate, "spikiness",       NULL);
	com->durability      = get_string(lstate, "durability",      NULL);
	com->match_type      = get_bool(lstate, "match_type",      0);

	vector_append(&((recipe*)(recipes.arr[recipes.cnt-1]))->comps,com);

	return 0;
}

int lcf_room(lua_State * lstate)
{
	room * rom;

	assert(lua_istable(lstate, 1));

	rom = malloc(sizeof(room));

	rom->max_height   = get_int(lstate, "max_height",  2);
	rom->max_width    = get_int(lstate, "max_width",  2);
	rom->min_height   = get_int(lstate, "min_height",  1);
	rom->min_width    = get_int(lstate, "min_width",  1);
	rom->shape        = get_int(lstate, "shape",  0);
	rom->features     = get_int(lstate, "features",  0);

	assure_world();
	add_to_gclass(lstate, world.grooms, rom);

	return 0;
}
