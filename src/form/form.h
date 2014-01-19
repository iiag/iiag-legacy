//
// form.h
//

struct form;

#ifndef FORM_FORM_H
#define FORM_FORM_H

#include <ncurses.h>

#define is_form \
	char * name; \
	int refs; \
	int weight; \
	chtype ch

// this type should never be declared
typedef struct form {
	is_form;
} form;

void form_initial(void *, chtype);
void * form_copy(void *);

#endif
