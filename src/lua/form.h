//
// lua/form.h
//

#ifndef LUA_FORM_H
#define LUA_FORM_H

#include <lua.h>

int lcf_creature(lua_State *);
int lcf_player(lua_State *);
int lcf_item(lua_State *);
int lcf_material(lua_State *);
int lcf_room(lua_State *);

#endif
