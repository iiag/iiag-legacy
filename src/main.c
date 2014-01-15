//
// main.c
//

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "world.h"
#include "object.h"
#include "display.h"

void moveplyr(int dx, int dy)
{
	obj_move(&PLYR, dx, dy);
}

int main(int argc, char ** argv)
{
	int c;

	srand(time(NULL));

	init_disp();
	init_world();

	zone_draw(PLYR.z);

	for (;;) {
		switch (c = wgetch(memoscr)) {
		case 'h': moveplyr(-1,  0); break;
		case 'j': moveplyr( 0,  1); break;
		case 'k': moveplyr( 0, -1); break;
		case 'l': moveplyr( 1,  0); break;
		case 'y': moveplyr(-1, -1); break;
		case 'u': moveplyr( 1, -1); break;
		case 'b': moveplyr(-1,  1); break;
		case 'n': moveplyr( 1,  1); break;
		case 'q': goto cleanup;
		default:
			memo("Unknown key press: %c (%d)", c, c);
		}
	}

cleanup:
	endwin();
	return 0;
}
