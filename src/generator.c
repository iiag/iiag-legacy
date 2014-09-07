//
// generator.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "world.h"
#include "generator.h"
#include "io/display.h"

gclass_t * new_gclass(gclass_t * par)
{
	gclass_t * cl = malloc(sizeof(gclass_t));

	cl->name = NULL;
	cl->lvl_cached = par == NULL ? 1 : par->lvl_cached;
	cl->max_freq = 0;
	cl->par = par;

	vector_init(&cl->elm);
	vector_init(&cl->sub);

	return cl;
}

gclass_t * get_gclass(const char * n, gclass_t * rt)
{
	int i, len;
	const char * s;
	gclass_t * cl;

	while (n != NULL && *n) {
		while (*n == '/') n++;
		s = n;
		while (*n && *n != '/') n++;

		for (i = 0; i < rt->sub.cnt; i++) {
			cl = rt->sub.arr[i];
			len = strlen(cl->name);
			len = len > n-s ? len : n-s;

			if (!strncmp(s, cl->name, len)) {
				rt = cl;
				break;
			}
		}

		if (i == rt->sub.cnt) {
			// create new gclass
			cl = new_gclass(rt);
			vector_append(&rt->sub, cl);

			cl->name = malloc(n - s + 1);
			memcpy(cl->name, s, n-s);
			cl->name[n-s] = 0;

			rt = cl;
		}
	}

	return rt;
}

void add_gelm(gclass_t * cl, int freq, int lvl, void * stf)
{
	gelm_t * e = malloc(sizeof(gelm_t));

	e->lvl = lvl;
	e->freq = freq;
	e->adj_freq = freq * (MAX_LEVEL - abs(lvl - cl->lvl_cached));
	e->stuff = stf;

	vector_append(&cl->elm, e);

	while (cl != NULL) {
		cl->max_freq += e->adj_freq;
		cl = cl->par;
	}
}

static int cache(gclass_t * cl, int lvl)
{
	gelm_t * e;
	int i, t = 0;

	if (cl->lvl_cached != lvl) {
		// adjust element frequencies
		for (i = 0; i < cl->elm.cnt; i++) {
			e = cl->elm.arr[i];
			e->adj_freq = e->freq * (MAX_LEVEL - abs(e->lvl - lvl));
			t += e->adj_freq;
		}

		// recurse on sub-classes
		for (i = 0; i < cl->sub.cnt; i++) {
			t += cache(cl->sub.arr[i], lvl);
		}

		cl->max_freq = t;
		cl->lvl_cached = lvl;
	}

	return cl->max_freq;
}

static void cache_tree(gclass_t * cl, int lvl)
{
	if (lvl != cl->lvl_cached) {
		cache(cl, lvl);

		while (cl != NULL) {
			if (cl->lvl_cached != lvl) {
				cl->lvl_cached = 0;
			}
			cl = cl->par;
		}
	}
}

static void * gensub(gclass_t * cl, int x)
{
	int i;
	gelm_t * elm;
	gclass_t * sub;

	// search through subclasses
	for (i = 0; i < cl->sub.cnt; i++) {
		sub = cl->sub.arr[i];
		x -= sub->max_freq;

		if (x <= 0) {
			return gensub(sub, -x);
		}
	}

	// search through elements
	for (i = 0; i < cl->elm.cnt; i++) {
		elm = cl->elm.arr[i];
		x -= elm->adj_freq;

		if (x <= 0) {
			return elm->stuff;
		}
	}

	assert(0);
	return NULL;
}

static void * gen(gclass_t * cl, int lvl, int x)
{
	cache_tree(cl, lvl);
	assert(cl->max_freq);
	return gensub(cl, x/*random() % cl->max_freq*/);
}

item * gen_item(gclass_t * cl, int lvl)
{
	gclass_t * mat_cl;
	item * it;
	int x,x2;

	x = random() % cl->max_freq;

	it = item_copy(gen(cl, lvl, x));

	if (it->mat_class != NULL) {
		mat_cl = get_gclass(it->mat_class, world.gmats);
		x2 = random() % mat_cl->max_freq;
		item_apply_mat(it, gen(mat_cl, lvl, x2));
	}

	info("Generated a %s", it->name);

	it->gen_id=x;
	it->gen_mat_id=x2;
	it->ch |= COLOR_PAIR(COLOR_ITEM);

	return it;
}

item * gen_item_from_id(gclass_t * cl, int lvl, int x, int x2)
{
	gclass_t * mat_cl;
	item * it;

	it = item_copy(gen(cl, lvl, x));

	if (it->mat_class != NULL) {
		mat_cl = get_gclass(it->mat_class, world.gmats);
		item_apply_mat(it, gen(mat_cl, lvl, x2));
	}

	it->gen_id=x;
	it->gen_mat_id=x2;
	it->ch |= COLOR_PAIR(COLOR_ITEM);

	return it;
}

creature * gen_crtr(gclass_t * cl, int lvl)
{
	int x;
	x = random() % cl->max_freq;
	return gen_crtr_from_id(cl,lvl,x);
}

creature * gen_crtr_from_id(gclass_t * cl, int lvl, int x)
{
	creature* c;

	c = crtr_copy(gen(cl, lvl, x));
	c->gen_id=x;
	c->ch |= COLOR_PAIR(COLOR_ENEMY);

	return c;
}

room * gen_room(gclass_t * cl, int lvl)
{
	int x;
	room * rom;

	x = random() % cl->max_freq;
	rom = malloc(sizeof(room));

	memcpy(rom,gen(cl, lvl, x),sizeof(room));

	return rom;
}
