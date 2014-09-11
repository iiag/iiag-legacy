#include <string.h>
#include <stdlib.h>
#include "creature.h"
#include "spells.h"
#include "io/display.h"


// TODO: Temporary testing function -> look into generics?
void test_fire(creature * caster, creature * target)
{
	target->health -= 10;

	if (target->health <= 0) {
		crtr_death(target, "fireball explosion");
		memo("The %s has been killed with fire!\n", crtr_name(target));
	} else {
		memo("The fireball hits the %s\n", crtr_name(target));
	}
}


// TODO: Load in from config file or something here, remove crappy default information
spell * spell_new()
{
	spell * sp = malloc(sizeof(spell));

	sp->caliber = 0;
	strcpy(sp->name = calloc(1,sizeof(char) * strlen("fireball")+1), "fireball");
	sp->effect = test_fire;

	return sp;
}

spell * spell_upgrade(spell * one, spell * two)
{


}


void spell_free(spell * sp)
{
	free(sp);
}

