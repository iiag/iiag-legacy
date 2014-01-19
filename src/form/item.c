//
// form/item.c
//

#include <stdlib.h>
#include "item.h"

iform * iform_new(item_type type, chtype ch)
{
	iform * f = malloc(sizeof(iform));

	form_initial(f, ch);
	f->type = type;

	return f;
}

void iform_free(iform * f)
{
	if (!--f->refs) free(f);
}
