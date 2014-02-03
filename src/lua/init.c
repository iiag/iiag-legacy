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

lua_State * prim_lstate;
const char * init_script = "script/init.lua";

static const struct {
	const char * name;
	lua_CFunction func;
} funcs[] = {
	{ "memo",   lcf_memo   },
	{ "wrlog",  lcf_wrlog  },
	{ "cform",  lcf_cform  },
	{ "player", lcf_player },
	{ "iform",  lcf_iform  },
};

void init_lua(void)
{
	int i;
	lua_State * lstate;

	lstate = luaL_newstate();
	luaL_openlibs(lstate);

	if (luaL_loadfile(lstate, init_script)) {
		end_disp();
		fprintf(stderr, "Failed to load init script: %s\n", lua_tostring(lstate, -1));
		exit(-1);
	}

	for (i = 0; i < sizeof(funcs) / sizeof(*funcs); i++) {
		lua_pushcfunction(lstate, funcs[i].func);
		lua_setglobal(lstate, funcs[i].name);
	}

	if (lua_pcall(lstate, 0, 0, 0)) {
		end_disp();
		fprintf(stderr, "Failed to execute init script: %s\n", lua_tostring(lstate, -1));
		exit(-1);
	}

	prim_lstate = lstate;
}

void cleanup_lua(void)
{
	lua_close(prim_lstate);
}
