//
// faction.h
//

#ifndef FACTION_H
#define FACTION_H

typedef struct relation relation;
typedef struct faction faction;

struct relation {
	int value;
	relation * next;
	const faction * to;
};

struct faction {
	char * name;
	relation * rels;
	faction * next;
};

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
