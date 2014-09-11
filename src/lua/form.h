//
// lua/form.h
//

#ifndef LUA_FORM_H
#define LUA_FORM_H

#include <lua.h>

int lcf_creature(lua_State *);
int lcf_player(lua_State *);
int lcf_item(lua_State *);
int lcf_room(lua_State *);
int lcf_i_type(lua_State *);
int lcf_begin_recipe(lua_State *);
int lcf_component(lua_State *);

#endif
