#include "recipe.h"
#include "log.h"
#include "io/display.h"
#include <stdlib.h>
#include <string.h>

vector_t item_types; //i_type
vector_t materials;  //char*
vector_t recipes;   //recipies

char* quality[QUALITY_NUM]={
	"Broken",
	"Awful",
	"Simple",
	"Average",
	"Neat",
	"Complicated",
	"Awesume",
	"COSI",
};

void init_recipes(){
	vector_init(&item_types);
	vector_init(&materials);
	vector_init(&recipes);
}

//this should be moved to util.
int find_str(vector_t* vec, const char * c){
	int x;
	for(x=0;x<vec->cnt;x++){
		if(!strcmp(vec->arr[x],c))
			return x;
	}

return -1;
}

int find_class(vector_t* vec, const char * c){
	int x;
	for(x=0;x<vec->cnt;x++){
		if(!strcmp(((i_class*)vec->arr[x])->name,c))
			return x;
	}

return -1;
}

int  is_class(int i, int i2, int match){
	if(i == i2)
		return 1;

	if(match)
		return 0;

	if(((i_class*)item_types.arr[i2])->type_of == -1)
		return 0;
	return is_class(i,((i_class*)item_types.arr[i2])->type_of,match);
}

static void apply_stat(int* i, int* sub, char* str){
	if(!str)
		return;

	char buf[11];
	int x;
	float num,num2;
	int op,buf_x;
	op=0;
	num=0;
	num2=0;
	buf_x=0;

	for(x=0; str[x] != 0; x++){
		if(buf_x<11 && (str[x]=='-' || str[x]=='.' || (str[x]>='0' && str[x]<='9'))){
			buf[buf_x++]=str[x];
			buf[buf_x]=0;
			num=atof(buf);
		}

		if(str[x]== 'f')
			num = *i;

		if(str[x]== 't')
			num = *sub;

		if(str[x] == '+' || str[x] == '*'){
			if(op == 0){
				num2 = num2 + num;
				buf_x=0;
			}
			if(op == 1){
				num2 = num2 * num;
				buf_x=0;
			}
			op=(str[x]=='*'?1:0);

		}

	}

	if(op == 0){
		num2 = num2 + num;
	}
	if(op == 1){
		num2 = num2 * num;
	}
	op=(str[x]=='*'?1:0);
	*i= (int)num2;
}


#define STAT_PARAM(x) &i->x,&sub->x,c->x
static void apply_comp(item* i, component* c, item* sub){
	if(c->mat)
	i->mat = sub->mat;

	apply_stat(STAT_PARAM(weight));
	apply_stat(STAT_PARAM(spikiness));
	apply_stat(STAT_PARAM(durability));
	apply_stat(STAT_PARAM(restore_health));
	apply_stat(STAT_PARAM(restore_stamina));
	apply_stat(STAT_PARAM(modify_attack));
	apply_stat(STAT_PARAM(modify_ac));

}

void recipe_make(inventory* i, recipe* r){

	item* ret = item_new(r->attrb,r->ch);

	ret->slot = r->slot;
	ret->iclass = r->iclass;
	ret->mat = r->mat;
	ret->tile = r->ch;

	ret->weight = r->weight;
	ret->spikiness = r->spikiness;
	ret->durability = r->durability;
	ret->restore_health = r->restore_health;
	ret->restore_stamina = r->restore_stamina;
	ret->modify_attack = r->modify_attack;
	ret->modify_ac = r->modify_ac;
	int x,y;

	for(x=0;x<r->comps.cnt;x++)
	for(y=0;y<i->size;y++){
		if(i->itms[y]){
			if(is_class(((component*)r->comps.arr[x])->iclass,i->itms[y]->iclass,((component*)r->comps.arr[x])->match_type)){
				apply_comp(ret,r->comps.arr[x],i->itms[y]);
				inv_rm(i->itms[y]);
				break;
			}
		}
	}

	ret->quality = rand()%QUALITY_NUM;

	item_gen_name(ret);

	memo("Thou hast created a %s.",ret->name);

	inv_clear(i);
	inv_add(i,ret);
}

