//
// trigger.c
///

#include <assert.h>
#include "log.h"
#include "trigger.h"
#include "lua/lua.h"

void trigger_pull(trigger tr, void * ud)
{
	if (tr) {
		lua_checkstack(prim_lstate, 3);
		lua_rawgeti(prim_lstate, LUA_REGISTRYINDEX, tr);
		lua_pushlightuserdata(prim_lstate, ud);

		if (lua_isfunction(prim_lstate, -2)) {
			if (lua_pcall(prim_lstate, 1, 0, 0)) {
				wrlog("Error in trigger: %s.\n", lua_tostring(prim_lstate, -1));
				lua_pop(prim_lstate, 1);
			}
		} else {
			wrlog("Error: Trigger index does not point to lua function.");
		}
	}
}
