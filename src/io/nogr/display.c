//
// io/nogr/display.c
//

#include <stdarg.h>
#include "input.h"
#include "display.h"
#include "../input.h"
#include "../display.h"

static void nogr_put(int x, int y, int tile)
{
	printf("Put %d at (%d, %d).\n", tile, x, y);
}

static void nogr_dim_update(int * x, int * y)
{
	// TODO there should be a better way to do this
	*x = 1000;
	*y = 1000;
}

static void nogr_memo(const char * fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);

	printf("Memo: ");
	vprintf(fmt, vl);
	puts("");

	va_end(vl);
}

static void nogr_statline(int line, const char * fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);

	printf("Status %d: ", line);
	vprintf(fmt, vl);
	puts("");

	va_end(vl);
}

void nogr_init(void)
{
	graphics_put = nogr_put;
	graphics_dim_update = nogr_dim_update;
	memo = nogr_memo;
	statline = nogr_statline;

	name_from_key  = nogr_name_from_key;
	key_from_name  = nogr_key_from_name;
	input_get_ctrl = nogr_get_ctrl;
	input_get_key  = nogr_get_key;
	input_prompt_dir = nogr_prompt_dir;
	input_prompt_inv = nogr_prompt_inv;
	input_prompt_equipped = nogr_prompt_equipped;
	input_prompt_string = nogr_prompt_string;
}

