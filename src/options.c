//
// options.c
//

#include "log.h"
#include "options.h"

unsigned options_set = 0;

void parse_options(int argc, char ** argv)
{
#define SET(X) options_set |= X; break;

	int i;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 's': SET(OPT_SHOW_ALL);
			default: wrlog("Unknown switch '%s'.", argv[i]);
			}
		} else {
			wrlog("Stray option string '%s'.", argv[i]);
		}
	}

#undef SET
}
