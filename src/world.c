//
// world.c
//

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "log.h"
#include "util.h"
#include "world.h"
#include "display.h"
#include "inventory.h"

const char * form_file = "script/forms";
static int form_line;

world_st world;

static void ff_error(char * fmt, ...)
{
	va_list vl;

	end_disp();

	va_start(vl, fmt);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, fmt, vl);
	fprintf(stderr, " (%s, line %d)\n", form_file, form_line);
	va_end(vl);

	exit(1);
}

static void igspaces(char ** buf)
{
redo:
	while (isspace(**buf)) {
		if (**buf == '\n') ++form_line;
		++*buf;
	}

	if (**buf == '#') {
		while (**buf != '\n') ++*buf;
		form_line++;
		goto redo;
	}
}

static int nexttok(char ** buf, char * delims, char ** tok)
{
	char * d;
	int l = 0;

	igspaces(buf);
	*tok = *buf;

	while (!isspace((*buf)[l])) {
		for (d = delims; *d != 0; d++) {
			if (*d == (*buf)[l]) return l;
		}
		l++;
	}
	return l;
}


static void expect(char ** buf, char c)
{
	igspaces(buf);
	if (**buf != c) {
		ff_error("Expected '%c', got '%c'", c, **buf);
	}
	++*buf;
}

static void load_forms(void)
{
#define REALLOC_SIZE 10

	form * fm;
	char dlm;
	char * buf, * orig;
	char * fld, * val;
	int lenf, lenv;

	world.form_count = 0;
	world.forms = NULL;

	orig = buf = read_file(form_file);
	if (buf == NULL) {
		end_disp();
		fprintf(stderr, "Error: Cannot load form file '%s'\n", form_file);
		exit(1);
	}

	while (*buf != 0) {
		igspaces(&buf);
		if (*buf == 0) break;

		switch (*buf++) {
		case 'u': fm = form_new(USELESS,  '?'); break;
		case 'c': fm = form_new(CREATURE, '?'); break;
		default: ff_error("Unknown form type '%c'", *buf);
		}

		expect(&buf, '{');

		do {
			lenf = nexttok(&buf, "=,", &fld);
			buf += lenf;

			expect(&buf, '=');

			lenv = nexttok(&buf, ",}", &val);
			buf += lenv;

			if (isspace(*buf)) {
				if (*buf == '\n') form_line++;
				*buf++ = 0;
				igspaces(&buf);
				dlm = *buf++;
			} else {
				dlm = *buf;
				*buf++ = 0;
			}

			if (!memcmp(fld, "weight", lenf)) {
				fm->weight = atoi(val);
			}
			else if (!memcmp(fld, "char", lenf)) {
				fm->ch = *val;
			}
			else if (!memcmp(fld, "name", lenf)) {
				fm->name = copy_str(val);
			}
			else {
				ff_error("Unknown field name");
			}

			if (!(world.form_count % REALLOC_SIZE)) {
				world.forms = realloc(world.forms, (world.form_count + REALLOC_SIZE) * sizeof(form *));
			}

			world.forms[world.form_count++] = fm;

		} while (dlm != '}');
	}

	free(orig);
}

void init_world(void)
{
	int x, y;

	world.zones = malloc(sizeof(zone *));
	load_forms();

	*world.zones = zone_new(80, 25);
	world.plyr.f = form_new(CREATURE, '@' | A_BOLD);
	world.plyr.inv = inv_new(500);
	world.plyr.f->weight = TILE_MAX_WEIGHT / 2;
	world.plyr.f->max_health = 10;
	world.plyr.health = 10;
	world.plyr.flags |= FL_NOFREE;

	do {
		x = rand() % world.zones[0]->width;
		y = rand() % world.zones[0]->height;
	} while (!inv_try(world.zones[0]->tiles[x][y], &world.plyr));

	obj_tele(&world.plyr, x, y, *world.zones);
}
