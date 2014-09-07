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
#include "../config.h"
#include "../io/display.h"

lua_State * prim_lstate;

static const struct {
	const char * name;
	lua_CFunction func;
} funcs[] = {
	{ "memo",     lcf_memo     },
	{ "debug",    lcf_debug    },
	{ "info",     lcf_info     },
	{ "notice",   lcf_notice   },
	{ "warning",  lcf_warning  },
	{ "error",    lcf_error    },
	{ "cform",    lcf_creature },
	{ "player",   lcf_player   },
	{ "iform",    lcf_item     },
	{ "material", lcf_material },
	{ "rform",    lcf_room     },
};

void init_lua(void)
{
	int i;
	lua_State * lstate;

	lstate = luaL_newstate();
	luaL_openlibs(lstate);

	if (config.lua_init != NULL) {
		if (luaL_loadfile(lstate, config.lua_init)) {
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
	}

	prim_lstate = lstate;
}

void cleanup_lua(void)
{
	lua_close(prim_lstate);
}
