//
// generator.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"

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
	int i;
	const char * s;
	gclass_t * cl;

	while (n != NULL && *n) {
		while (*n == '/') n++;
		s = n;
		while (*n && *n != '/') n++;

		for (i = 0; i < rt->sub.cnt; i++) {
			cl = rt->sub.arr[i];

			if (!strncmp(s, cl->name, n-s)) {
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

		if (x < 0) {
			return gensub(sub, -x);
		}
	}

	// search through elements
	for (i = 0; i < cl->elm.cnt; i++) {
		elm = cl->elm.arr[i];
		x -= elm->adj_freq;

		if (x < 0) {
			return elm->stuff;
		}
	}

	assert(0);
	return NULL;
}

static void * gen(gclass_t * cl, int lvl)
{
	cache_tree(cl, lvl);
	return gensub(cl, random() % cl->max_freq);
}

item * gen_item(gclass_t * cl, int lvl)
{
	//TODO
	return item_copy(gen(cl, lvl));
}

creature * gen_crtr(gclass_t * cl, int lvl)
{
	return crtr_copy(gen(cl, lvl));
}
