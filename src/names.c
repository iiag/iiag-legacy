//
// names.c
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "names.h"
#include "log.h"

static void load_syl(vector_t * v, const char * dir, const char * file)
{
	FILE * f;
	char str[11];
	char * cpy;
	char * path = malloc(strlen(dir) + 12);

	str[10] = 0;
	strcpy(path, dir);
	strcat(path, "/");
	strcat(path, file);

	vector_init(v);

	f = fopen(path, "rb");
	if (f != NULL) {
		for (;;) {
			fgets(str, 10, f);
			if (feof(f)) break;

			if (str[strlen(str)-1] == '\n') str[strlen(str)-1] = 0;

			cpy = malloc(strlen(str) + 1);
			strcpy(cpy, str);
			vector_append(v, cpy);
		}

		fclose(f);
	} else {
		// This is relatively normal, but it could be a typo in a file name
		notice("Could not open file: %s", path);
	}
}

ethnicity * load_ethnicity(const char * dir)
{
	ethnicity * eth = malloc(sizeof(ethnicity));

	info("Loading ethnicity from %s", dir);

	eth->gen_max = 3;
	load_syl(&eth->gen_syl, dir, "gen");
	load_syl(&eth->pre_syl[NAME_PLACE],   dir, "place_pre");
	load_syl(&eth->pre_syl[NAME_PERSON],  dir, "person_pre");
	load_syl(&eth->suff_syl[NAME_PLACE],  dir, "place_suff");
	load_syl(&eth->suff_syl[NAME_PERSON], dir, "person_suff");

	return eth;
}

static void append(char ** str, const vector_t * syl)
{
	char * app;

	app = syl->arr[random() % syl->cnt];
	*str = realloc(*str, strlen(*str) + strlen(app) + 1);
	strcat(*str, app);
}

static char * gen_name(const ethnicity * e, int type)
{
	int i, max;
	char * name = malloc(1);

	name[0] = 0;

	if (e->pre_syl[type].cnt) {
		append(&name, &e->pre_syl[type]);
	}

	max = random() % e->gen_max + 1;
	for (i = 0; i < max; i++) {
		append(&name, &e->gen_syl);
	}

	if (e->suff_syl[type].cnt) {
		append(&name, &e->suff_syl[type]);
	}

	name[0] = toupper(name[0]);
	return name;
}

char * place_name(const ethnicity * e)
{
	return gen_name(e, NAME_PLACE);
}

char * person_name(const ethnicity * e)
{
	return gen_name(e, NAME_PERSON);
}
