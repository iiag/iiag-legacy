//
// form.c
//

#include <stdlib.h>
#include "form.h"

#define DEF_HEALTH 10

form * form_new(obj_type type, chtype ch)
{
	form * f  = malloc(sizeof(form));
	f->flags  = (type == CREATURE) ? FL_HASINV : 0;
	f->type   = type;
	f->name   = NULL;
	f->ch     = ch;
	f->refs   = 1;
	f->weight = 1;
	f->max_health = DEF_HEALTH;
	return f;
}

form * form_copy(form * f)
{
	f->refs++;
	return f;
}

void form_free(form * f)
{
	f->refs--;
	if (!f->refs) {
		free(f->name);
		free(f);
	}
}
