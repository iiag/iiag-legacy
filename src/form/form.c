//
// form/form.c
//

#include "form.h"

typedef struct {
	is_form;
} form;

void form_initial(void * ptr, chtype ch)
{
	form * f = ptr;
	f->freq = 1;
	f->refs = 1;
	f->name = NULL;
	f->weight = 1;
	f->ch = ch;
}

void * form_assign(void * ptr)
{
	form * f = ptr;
	f->refs++;
	return f;
}
