//
// lua/io.c
//

#include <assert.h>
#include "io.h"
#include "../log.h"
#include "../io/display.h"

static const char * stringify(lua_State * lstate)
{
	int n = lua_gettop(lstate);
	lua_concat(lstate, n);

	assert(lua_isstring(lstate, 1));
	return lua_tostring(lstate, 1);
}

#define GEN_LOGLEVEL(LEVEL) int lcf_##LEVEL(lua_State * lstate)\
{\
	LEVEL(stringify(lstate));\
	return 0;\
}

GEN_LOGLEVEL(debug)
GEN_LOGLEVEL(info)
GEN_LOGLEVEL(notice)
GEN_LOGLEVEL(warning)
GEN_LOGLEVEL(error)

#undef GEN_LOGLEVEL

int lcf_memo(lua_State * lstate)
{
	memo(stringify(lstate));
	return 0;
}
