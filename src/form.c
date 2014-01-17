//
// form.c
//

#include <stdlib.h>
#include "form.h"

form * form_new(obj_type type, chtype ch)
{
	form * f = malloc(sizeof(form));
	f->type = type;
	f->name = NULL;
	f->ch = ch;
	f->refs = 1;
	f->weight = 1;
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
