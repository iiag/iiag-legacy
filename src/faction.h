//
// faction.h
//

struct relation;
struct faction;

#ifndef FACTION_H
#define FACTION_H

typedef struct relation {
	int value;
	struct relation * next;
	const struct faction * to;
} relation;

typedef struct faction {
	char * name;
	struct relation * rels;
	struct faction * next;
} faction;

//
// Retrieves a faction associated with a given identifier. If the faction does
//  not exist, it is created.
//
faction * fctn_get(const char *);

//
// This faction determines the relationship between 2 factions. The default is 0
//  A faction to itself will result in 1000 for now
//
int fctn_relto(const faction *, const faction *);

//
// This faction additivly modifies a given faction's relationship to another.
//
void fctn_modrel(faction *, const faction *, int);

#endif
