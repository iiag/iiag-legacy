//
// lua/io.c
//

#include <assert.h>
#include "io.h"
#include "../log.h"
#include "../display.h"

const char * stringify(lua_State * lstate)
{
	int n = lua_gettop(lstate);
	lua_concat(lstate, n);

	assert(lua_isstring(lstate, 1));
	return lua_tostring(lstate, 1);
}

int lcf_wrlog(lua_State * lstate)
{
	wrlog(stringify(lstate));
	return 0;
}

int lcf_memo(lua_State * lstate)
{
	memo(stringify(lstate));
	return 0;
}
