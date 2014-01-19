//
// form/form.c
//

#include "form.h"

void form_initial(void * ptr, chtype ch)
{
	form * f = ptr;
	f->refs = 1;
	f->name = NULL;
	f->weight = 1;
	f->ch = ch;
}

void * form_copy(void * ptr)
{
	form * f = ptr;
	f->refs++;
	return f;
}
