//
// crtr.c
//

#include <stdlib.h>
#include "crtr.h"

cform * cform_new(chtype ch)
{
	cform * cf = malloc(sizeof(cform));

	form_initial(cf, ch);
	cf->max_health = 10;
	cf->max_stamina = 5000;
	cf->def_attack = 1;
	cf->def_ac = 1;

	return cf;
}

void cform_free(cform * c)
{
	if (!--c->refs) free(c);
}
