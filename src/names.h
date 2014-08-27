//
// names.h
//

#ifndef NAMES_H
#define NAMES_H

#include "util.h"

#define NAME_PLACE  0
#define NAME_PERSON 1

typedef struct {
	int gen_max;
	vector_t gen_syl;
	vector_t pre_syl[2];
	vector_t suff_syl[2];
} ethnicity;

//
// This function loads ethnicity name data from a directory
//  The significant files in that directory are: gen, place_pre, place_suff, person_pre, person_suff
//
ethnicity * load_ethnicity(const char * directory);

//
// These functions generate an ethnicity
//
char * place_name (const ethnicity *);
char * person_name(const ethnicity *);

#endif
