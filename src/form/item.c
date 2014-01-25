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
	f->u.cn.restore_health = 0;
	f->u.cn.restore_stamina = 0;
	f->u.eq.modify_attack = 0;
	f->u.eq.modify_ac = 0;

	return f;
}

void iform_free(iform * f)
{
	if (!--f->refs) free(f);
}
