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

cform * cform_new(chtype);
void cform_free(cform *);

#endif
