
#include "spells.h"

spell * spell_new()
{
	spell * sp = malloc(sizeof(spell));

	char * name;
	int level;

	sp->caliber = 0;

}

spell * spell_upgrade(spell * one, spell * two)
{


}


void spell_free(spell * sp)
{
	free(sp);
}

