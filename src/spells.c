#include <string.h>
#include <stdlib.h>
#include "creature.h"
#include "spells.h"


// TODO: Temporary testing function -> look into generics?
void test_fire(creature * caster, creature * target)
{
	target->health -= 20;

	if (target->health <= 0) {
		memo("The fireball killed the %s!\n", crtr_name(target));
		crtr_death(target, "projectile impact"); // TODO: spell cast?
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

