//
// recipe.h
//

#ifndef RECIPE_H
#define RECIPE_H
#include "util.h"
#include "item.h"
#include <ncurses.h>

typedef struct recipe recipe;
typedef struct i_class i_class;
typedef struct component component;

extern vector_t item_types; //i_type
extern vector_t materials;  //char*
extern vector_t recipes;   //recipies

#define QUALITY_NUM 8
extern char* quality[QUALITY_NUM];    //quality of craft

#define STATS(x) \
x weight; \
x spikiness; \
x durability; \
x restore_health; \
x restore_stamina; \
x modify_attack; \
x modify_ac;

struct recipe{
	int iclass;
	int attrb; //consumable, equipable
	int slot;
	int ch;
	int mat;

	int obj_type; //type of tile object used to craft
	int obj_arg;  //arg to tile object for crafting

	STATS(int)

	vector_t comps;
};

struct component {
	int iclass;
	char* mat;  //to define material inheritance
	STATS(char*)
};

struct i_class {
	char* name;
	int type_of;
};


void init_recipes();
//TODO implement this function
void free_recipes();
int  find_str(vector_t* vec, char * c);
int  find_class(vector_t* vec, char * c);
void recipe_make(inventory* i, recipe* r);

//checks if i2 is of type i
int  is_class(int i, int i2);

#endif
