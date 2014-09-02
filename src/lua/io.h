//
// lua/io.h
//

#ifndef LUA_IO_H
#define LUA_IO_H

#include <lua.h>

int lcf_debug(lua_State *);
int lcf_info(lua_State *);
int lcf_notice(lua_State *);
int lcf_warning(lua_State *);
int lcf_error(lua_State *);
int lcf_memo(lua_State *);

#endif
