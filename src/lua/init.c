//
// lua/init.c
//

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include "io.h"
#include "lua.h"
#include "form.h"
#include "../log.h"
#include "../display.h"

const char * init_script = "script/init.lua";

static void reg_all(lua_State * lstate)
{
#define RFUNC(N, F) lua_pushcfunction(lstate, F); lua_setglobal(lstate, N)

	RFUNC("memo",  lcf_memo);
	RFUNC("wrlog", lcf_wrlog);
	RFUNC("cform", lcf_cform);
	RFUNC("iform", lcf_iform);

#undef RFUNC
}

void init_lua(void)
{
	lua_State * lstate;

	lstate = luaL_newstate();
	luaL_openlibs(lstate);

	if (luaL_loadfile(lstate, init_script)) {
		end_disp();
		fprintf(stderr, "Failed to load init script: %s\n", lua_tostring(lstate, -1));
		exit(-1);
	}

	reg_all(lstate);

	if (lua_pcall(lstate, 0, 0, 0)) {
		end_disp();
		fprintf(stderr, "Failed to execute init script: %s\n", lua_tostring(lstate, -1));
		exit(-1);
	}

	lua_close(lstate);
}
