//
// io/ncurses/keys.c
//

#include <string.h>
#include "input.h"
#include "local.h"

#ifdef WITH_NCURSES

struct {
	char * name;
	int key;
} skeys[] = {
	{ "space",  ' ' },
	{ "left",   KEY_LEFT  },
	{ "right",  KEY_RIGHT },
	{ "up",     KEY_UP    },
	{ "down",   KEY_DOWN  },
	{ "return", '\n' },
};

const char * nc_name_from_key(int key)
{
	static char tmp[2] = "\0\0";
	int i;

	for (i = 0; i < sizeof(skeys) / sizeof(*skeys); i++) {
		if (skeys[i].key == key) {
			return skeys[i].name;
		}
	}

	tmp[0] = key;
	return tmp;
}

int nc_key_from_name(const char * name)
{
	int i;

	for (i = 0; i < sizeof(skeys) / sizeof(*skeys); i++) {
		if (!strcmp(name, skeys[i].name)) {
			return skeys[i].key;
		}
	}

	return name[0];
}

#endif

