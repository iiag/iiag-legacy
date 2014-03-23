//
// form/crtr.h
//

struct cform;

#ifndef FORM_CRTR_H
#define FORM_CRTR_H

#include "form.h"
#include "../trigger.h"

typedef struct cform {
	is_form;
	int max_health;
	int max_stamina;
	int def_attack;
	int def_ac;

	trigger on_spawn;
} cform;

//
// Creates a new cform, to be freed with cform_free
//
cform * cform_new(chtype);

//
// Frees a cform created with cform_new or assigned with form_assign
//
void cform_free(cform *);

#endif
