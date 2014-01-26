//
// form/item.c
//

#include <stdlib.h>
#include "item.h"

iform * iform_new(unsigned type, chtype ch)
{
	iform * f = malloc(sizeof(iform));

	form_initial(f, ch);
	f->type = type;
	f->restore_health = 0;
	f->restore_stamina = 0;
	f->modify_attack = 0;
	f->modify_ac = 0;
	f->slot = 0;

	return f;
}

void iform_free(iform * f)
{
	if (!--f->refs) free(f);
}
