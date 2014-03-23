//
// form.h
//

struct form;

#ifndef FORM_FORM_H
#define FORM_FORM_H

#include <ncurses.h>

// Placed before properties in all form subclasses
#define is_form \
	char * name; \
	int refs; \
	int weight; \
	int freq; \
	chtype ch

//
// Initializes a form
// To be called in subclass constructors
//
void form_initial(void *, chtype);

//
// Handles reference counting for forms
// To be called on all permenant assignments of forms
//
void * form_assign(void *);

#endif
