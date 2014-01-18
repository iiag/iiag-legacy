//
// form.h
//

#ifndef FORM_H
#define FORM_H

#include <ncurses.h>

#define FL_HASINV 1

typedef enum {
	NONE = -1,
	USELESS,
	CREATURE,
} obj_type;

typedef struct {
	unsigned flags;
	int refs;
	int weight;
	char * name;
	obj_type type;
	chtype ch;
} form;

form * form_new(obj_type, chtype);
form * form_copy(form *);
void form_free(form *);

#endif
