#include "sol/ast.h"
#include "sol/cdata.h"
#include "faction.h"
#include "creature.h"
#include "util.h"
#include "log.h"
#include "solint.h"
#include "config.h"
#include "room.h"
#include "recipe.h"
#include "world.h"
#include "io/display.h"
#include "tileset.h"

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

sol_state_t *sol_state=NULL;

sol_object_t *sol_crtr_specs=NULL;
sol_object_t *sol_item_specs=NULL;

void init_sol(void) {
#define state sol_state
    stmt_node *program;
    FILE *f;

    debug("Initializing Sol interface...");

    if(state) {
        warning("Potential double-init of Sol");
        return;
    }

    state = malloc(sizeof(sol_state_t));
    sol_state_init(state);

    //Oops, we replace sol's "debug" library with the "debug" function. Let's make it
    //accessible through other means...

    sol_state_assign_name(state, "sol_debug", sol_state_resolve_name(state, "debug"));

    sol_state_assign_name(state, "creature", sol_new_cfunc(state, solcf_creature));
    sol_state_assign_name(state, "player", sol_new_cfunc(state, solcf_player));
    sol_state_assign_name(state, "iform", sol_new_cfunc(state, solcf_item));
    sol_state_assign_name(state, "room", sol_new_cfunc(state, solcf_room));
    sol_state_assign_name(state, "iclass", sol_new_cfunc(state, solcf_i_type));
    sol_state_assign_name(state, "recipe", sol_new_cfunc(state, solcf_begin_recipe));
    sol_state_assign_name(state, "component", sol_new_cfunc(state, solcf_component));
    sol_state_assign_name(state, "debug", sol_new_cfunc(state, solcf_debug));
    sol_state_assign_name(state, "info", sol_new_cfunc(state, solcf_info));
    sol_state_assign_name(state, "notice", sol_new_cfunc(state, solcf_notice));
    sol_state_assign_name(state, "warning", sol_new_cfunc(state, solcf_warning));
    sol_state_assign_name(state, "error", sol_new_cfunc(state, solcf_error));
    sol_state_assign_name(state, "fatal", sol_new_cfunc(state, solcf_fatal));
    sol_state_assign_name(state, "memo", sol_new_cfunc(state, solcf_memo));

    sol_crtr_specs = sol_new_cstruct_specs(state);
    sol_cstruct_add_member_name(state, sol_crtr_specs, "tile", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "refs", SOL_MT_INT, offsetof(creature, refs));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "deceased", SOL_MT_INT, offsetof(creature, deceased));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "step", SOL_MT_INT, offsetof(creature, step));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "z_ind", SOL_MT_INT, offsetof(creature, z_ind));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "level", SOL_MT_INT, offsetof(creature, level));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "xp", SOL_MT_INT, offsetof(creature, xp));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "need_xp", SOL_MT_INT, offsetof(creature, need_xp));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "health", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "stamina", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "max_health", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "max_stamina", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "attack", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "ac", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "sight", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "reflex", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "throw", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "speed", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "stance", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "gen_id", SOL_MT_INT, offsetof(creature, tile));
    sol_cstruct_add_member_name(state, sol_crtr_specs, "ai", SOL_MT_INT, offsetof(creature, tile));

    f = fopen(config.sol_init, "r");
    if(!f) fatal("Failed to open script init file");

    program = sol_compile_file(f);
    if(!program) fatal("Script file syntax error");

    debug("Executing Sol init scripts...");

    sol_exec(state, program);

    if(sol_has_error(state)) {
        error("Error occured while running init scripts:");
        if(sol_is_string(state->error)) fatal(state->error->str);
        error("Error follows in stdout");
        ob_print(state->error);
        fatal("Sol error occured. Stop.");
    }

    debug("Sol initialization completed.");
#undef state
}

int get_int(sol_state_t *state, sol_object_t *map, char *name, int def) {
    sol_object_t *val = sol_map_get_name(state, map, name), *vint;
    int res;

    if(sol_is_none(state, val)) {
        sol_obj_free(val);
        return def;
    }

    vint = sol_cast_int(state, val);
    res = vint->ival;
    sol_obj_free(vint);
    sol_obj_free(val);
    return res;
}

#define get_bool get_int

char *get_string(sol_state_t *state, sol_object_t *map, char *name, char *def) {
    sol_object_t *val = sol_map_get_name(state, map, name), *vstr;
    char *res;

    if(sol_is_none(state, val)) {
        sol_obj_free(val);
        if(def) return strdup(def);
        return NULL;
    }
    vstr = sol_cast_string(state, val);
    res = strdup(vstr->str);
    sol_obj_free(val);
    sol_obj_free(vstr);
    return res;
}

const char *get_cstring(sol_state_t *state, sol_object_t *map, char *name, const char *def) {
    sol_object_t *val = sol_map_get_name(state, map, name), *vstr;
    char *res;

    if(sol_is_none(state, val)) {
        sol_obj_free(val);
        return def;
    }
    vstr = sol_cast_string(state, val);
    res = strdup(vstr->str);
    sol_obj_free(val);
    sol_obj_free(vstr);
    return res;
}

int get_slot(sol_state_t *state, sol_object_t *map, char *name, int def) {
    char *s = get_string(state, map, name, "INVALID_SLOT");
    int i;

    for(i=0; i<MAX_SLOTS; i++) {
        if(!strcmp(s, slot_names[i])) {
            def = i;
            break;
        }
    }

    free(s);
    return def;
}

faction *get_faction(sol_state_t *state, sol_object_t *map, char *name, faction *def) {
    char *s = get_string(state, map, name, "INVALID_FACTION");

    if(!strcmp(s, "INVALID_FACTION")) {
        free(s);
        return def;
    }

    def = fctn_get(s);
    free(s);
    return def;
}

sol_object_t *get_trigger(sol_state_t *state, sol_object_t *map, char *name, sol_object_t *def) {
    sol_object_t *func = sol_map_get_name(state, map, name);

    if(func->ops->call && func->ops->call != sol_f_not_impl) def = sol_incref(func);

    sol_obj_free(func);
    return def;
}

static void set_creature(sol_state_t * state, sol_object_t *map, creature * cf)
{
	cf->generic_name = get_string(state, map, "name", cf->generic_name);
	cf->health      = get_int(state, map, "health",      cf->health);
	cf->stamina     = get_int(state, map, "stamina",     cf->stamina);
	cf->max_health  = get_int(state, map, "max_health",  cf->max_health);
	cf->max_stamina = get_int(state, map, "max_stamina", cf->max_stamina);
	cf->attack      = get_int(state, map, "attack",      cf->attack);
	cf->ac          = get_int(state, map, "ac",          cf->ac);
	cf->sight       = get_int(state, map, "sight",       cf->sight);
	cf->speed       = get_int(state, map, "speed",       cf->speed);
	cf->fctn        = get_faction(state, map, "faction", cf->fctn);
	cf->on_spawn.sol_func = get_trigger(state, map, "on_spawn", cf->on_spawn.sol_func);
	cf->on_death.sol_func = get_trigger(state, map, "on_death", cf->on_death.sol_func);
	cf->on_lvlup.sol_func = get_trigger(state, map, "on_lvlup", cf->on_lvlup.sol_func);
}

static void add_to_gclass(sol_state_t * state, sol_object_t *map, gclass_t * root, void * stf)
{
	int freq, lvl;
	const char * cl_name;
	gclass_t * cl;

	freq = get_int(state, map, "freq",  0);
	lvl  = get_int(state, map, "level", 1);
	cl_name = get_cstring(state, map, "class", "");
	cl = get_gclass(cl_name, root);

	add_gelm(cl, freq, lvl, stf);
}

sol_object_t *solcf_creature(sol_state_t * state, sol_object_t *args)
{
	creature * c;
	sol_object_t *map = sol_list_get_index(state, args, 0);

	assure_world();

	c = crtr_new(get_int(state, map, "tile", TILE_UNKNOWN));
	set_creature(state, map, c);

	assure_world();
	add_to_gclass(state, map, world.gcrtrs, c);

    sol_obj_free(map);
	return sol_incref(state->None);
}

sol_object_t *solcf_player(sol_state_t * state, sol_object_t *args)
{
    sol_object_t *map = sol_list_get_index(state, args, 0);

	assure_world();

	set_creature(state, map, &world.plyr);
	world.plyr.tile = get_int(state, map, "tile", world.plyr.tile);

    sol_obj_free(map);
	return sol_incref(state->None);
}

sol_object_t *solcf_item(sol_state_t * state, sol_object_t *args)
{
	item * it;
    sol_object_t *map = sol_list_get_index(state, args, 0);

	it = item_new(0, get_int(state, map, "tile", TILE_UNKNOWN));

	//it->name = get_string(state, "name", it->name);
	//it->mat_class = get_string(state, "material", it->mat_class);

	char* tmp;
	tmp = get_string(state, map, "iclass", NULL);
	if(tmp){
		it->iclass=find_class(&item_types,tmp);
		info("item name %i %s",it->iclass,tmp);
		free(tmp);
	}

	tmp = get_string(state, map, "material", NULL);
	if(tmp){
		it->mat=find_str(&materials,tmp);
		if(it->mat == -1){
			vector_append(&materials,tmp);
			it->mat = materials.cnt-1;
		}else
		free(tmp);
	}

	it->restore_health  = get_int(state, map, "restore_health",  it->restore_health);
	it->restore_stamina = get_int(state, map, "restore_stamina", it->restore_stamina);
	it->modify_attack   = get_int(state, map, "modify_attack",   it->modify_attack);
	it->modify_ac       = get_int(state, map, "modify_ac",       it->modify_ac);
	it->weight          = get_int(state, map, "weight",          it->weight);
	it->spikiness       = get_int(state, map, "spikiness",       it->spikiness);
	it->type = (it->type & ~ITEM_CONSUMABLE) | (get_bool(state, map, "consumable", it->type & ITEM_CONSUMABLE) << ITEM_CONSUMABLE_SFT);
	it->type = (it->type & ~ITEM_EQUIPABLE)  | (get_bool(state, map, "equipable",  it->type & ITEM_EQUIPABLE)  << ITEM_EQUIPABLE_SFT);
	it->slot = get_slot(state, map, "slot", it->slot);

	it->quality = rand()%QUALITY_NUM;
	item_gen_name(it);

	assure_world();
	add_to_gclass(state, map, world.gitems, it);

    sol_obj_free(map);
	return sol_incref(state->None);
}

sol_object_t *solcf_i_type(sol_state_t * state, sol_object_t *args)
{
	sol_object_t *map = sol_list_get_index(state, args, 0);

	char *tmp, *sub;
	i_class* type;

	tmp = get_string(state, map, "class", NULL);
	sub = get_string(state, map, "type_of", NULL);

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
	sol_obj_free(map);
	return sol_incref(state->None);
}

sol_object_t *solcf_begin_recipe(sol_state_t * state, sol_object_t *args)
{
	recipe * res;
	sol_object_t *map = sol_list_get_index(state, args, 0);

	res = malloc(sizeof(recipe));
	vector_init(&res->comps);

	int x;
	const char * tmp;

	tmp = get_cstring(state, map, "class", NULL);

	if(tmp == NULL) {
        sol_obj_free(map);
		return sol_incref(state->None);
    }

	res->iclass = find_class(&item_types,tmp);

	tmp = get_cstring(state, map, "made_by", NULL);
	res->obj_type=-1;
	if(tmp)
		for(x=0;x<OBJ_NUM;x++)
			if(!strcmp(tmp,tile_obj_names[x]))
				res->obj_type=x;

	res->obj_arg = get_int(state, map, "made_with",  0);

	tmp = get_cstring(state, map, "mat", NULL);
	if(tmp)
		res->mat=find_str(&materials,tmp);
	else
		res->mat=-1;

	//base stats
	res->restore_health  = get_int(state, map, "restore_health",  0);
	res->restore_stamina = get_int(state, map, "restore_stamina", 0);
	res->modify_attack   = get_int(state, map, "modify_attack",   0);
	res->modify_ac       = get_int(state, map, "modify_ac",       0);
	res->weight          = get_int(state, map, "weight",          0);
	res->spikiness       = get_int(state, map, "spikiness",       0);
	res->durability       = get_int(state, map, "durability",     0);

	res->attrb =0;
	res->attrb = (res->attrb & ~ITEM_CONSUMABLE) | (get_bool(state, map, "consumable", res->attrb & ITEM_CONSUMABLE) << ITEM_CONSUMABLE_SFT);
	res->attrb = (res->attrb & ~ITEM_EQUIPABLE)  | (get_bool(state, map, "equipable",  res->attrb & ITEM_EQUIPABLE)  << ITEM_EQUIPABLE_SFT);
	res->slot = get_slot(state, map, "slot", 0);
	res->ch = get_int(state, map, "tile", TILE_UNKNOWN);


	vector_append(&recipes,res);

    sol_obj_free(map);
	return sol_incref(state->None);
}

sol_object_t *solcf_component(sol_state_t * state, sol_object_t *args)
{
	component * com;
	sol_object_t *map = sol_list_get_index(state, args, 0);

	com = malloc(sizeof(component));

	char * tmp;

	tmp = get_string(state, map, "class", NULL);

	if(tmp == NULL) {
        sol_obj_free(map);
		return sol_incref(state->None);
    }

	com->iclass = find_class(&item_types,tmp);

	com->mat = get_string(state, map, "mat", NULL);

	//base stats
	com->restore_health  = get_string(state, map, "restore_health",  NULL);
	com->restore_stamina = get_string(state, map, "restore_stamina", NULL);
	com->modify_attack   = get_string(state, map, "modify_attack",   NULL);
	com->modify_ac       = get_string(state, map, "modify_ac",       NULL);
	com->weight          = get_string(state, map, "weight",          NULL);
	com->spikiness       = get_string(state, map, "spikiness",       NULL);
	com->durability      = get_string(state, map, "durability",      NULL);
	com->match_type      = get_bool(state, map, "match_type",      0);

	vector_append(&((recipe*)(recipes.arr[recipes.cnt-1]))->comps,com);

    sol_obj_free(map);
	return sol_incref(state->None);
}

sol_object_t *solcf_room(sol_state_t * state, sol_object_t *args)
{
	room * rom;
    sol_object_t *map = sol_list_get_index(state, args, 0);

	rom = malloc(sizeof(room));

	rom->max_height   = get_int(state, map, "max_height",  2);
	rom->max_width    = get_int(state, map, "max_width",  2);
	rom->min_height   = get_int(state, map, "min_height",  1);
	rom->min_width    = get_int(state, map, "min_width",  1);
	rom->shape        = get_int(state, map, "shape",  0);
	rom->features     = get_int(state, map, "features",  0);

	assure_world();
	add_to_gclass(state, map, world.grooms, rom);

    sol_obj_free(map);
	return sol_incref(state->None);
}

#define GEN_LOGLEVEL(LEVEL) sol_object_t *solcf_##LEVEL(sol_state_t *state, sol_object_t *args)\
{\
    sol_object_t *val = sol_list_get_index(state, args, 0);\
    sol_object_t *vstr = sol_cast_string(state, val);\
    LEVEL(vstr->str);\
    sol_obj_free(val);\
    sol_obj_free(vstr);\
    return sol_incref(state->None);\
}

GEN_LOGLEVEL(debug);
GEN_LOGLEVEL(info);
GEN_LOGLEVEL(notice);
GEN_LOGLEVEL(warning);
GEN_LOGLEVEL(error);
GEN_LOGLEVEL(fatal);
GEN_LOGLEVEL(memo); // Ell Oh Ell

#undef GEN_LOGLEVEL
