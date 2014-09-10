#include <stdlib.h>
#include "library.h"
#include "io/display.h"

library * lib_new(int max)
{
	library * lib = malloc(sizeof(library));
	lib->intelligence = max;
	lib->size = 0;
	lib->caliber = 0;
	lib->spls = NULL;
	return lib;
}

void lib_free(library * lib)
{
	int i;

	for (i = 0; i < lib->size; i++) {
		if (lib->spls[i] != NULL) {
			spell_free(lib->spls[i]);
		}
	}

	free(lib->spls);
	free(lib);
}

int lib_add(library * lib, spell * sp)
{
#define REALLOC_SIZE 8
	int i;

	if (!lib_try(lib, sp)) return -1;
	lib->caliber += sp->caliber;
	
	for (i = 0; i < lib->size; i++) {
		if (lib->spls[i] == NULL) {
			lib->spls[i] = sp;
			return i;
		}
	}

	if (lib->size % REALLOC_SIZE == 0) {
		lib->spls = realloc(lib->spls,
			(lib->size + REALLOC_SIZE) * sizeof(spell *)
		);
	}
	lib->spls[lib->size] = sp;

	for (i = 1; i < REALLOC_SIZE; i++) {
		lib->spls[i + lib->size] = NULL;
	}

	lib->size += REALLOC_SIZE;
	return lib->size - REALLOC_SIZE;
}

int lib_try(library * lib, spell * sp)
{
	if (lib->intelligence == INFINITE) return 1;
	return sp->caliber + lib->caliber <= lib->intelligence;
}

spell * lib_rm(library * lib, int i)
{
	if (lib->spls[i] == NULL) return NULL;

	spell * ret = lib->spls[i];
	lib->caliber -= lib->spls[i]->caliber;
	lib->spls[i] = NULL;

	return ret;
}

int lib_count(library * lib)
{
	int i;
	int count = 0;
	
	for (i = 0; i < lib->size; i++) {
		if (lib->spls[i] != NULL)
			count++;
	}

	return count;
}

void lib_clear(library * lib)
{
	int i;
	for (i = 0; i < lib->size; i++) {
		if (lib->spls[i] != NULL) continue;
		spell_free(lib->spls[i]);
		lib->spls[i] = NULL;
	}
	lib->caliber = 0;
}

void prompt_lib(char * c, int * dx, int * dy) {} // TODO: Write this!




