// TODO: Comment this file
// library.h
//

#ifndef LIBRARY_H
#define LIBRARY_H

typedef struct library library;

#include "spells.h"
#include "creature.h"

struct library {
	int size;
	int caliber;
	int intelligence;
	spell ** spls;
};

library * lib_new(int);

void lib_free(library *);

int lib_add(library *, spell *);

int lib_try(library *, spell *);

int lib_count(library* lib);

void lib_clear(library * lib);

void lib_empty(library* lib);

spell * lib_rm(library *, int);


#endif
