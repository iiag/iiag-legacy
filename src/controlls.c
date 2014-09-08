//
//controlls.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <dirent.h>
#include "player.h"
#include "io/display.h"
#include "config.h"
#include "controlls.h"

dirent_name * ent_name;

//control mapping
struct str_pairs controls[]={

	// movement
	{ "Move UP",          "CTRL_UP",     'k' },
	{ "Move Down",        "CTRL_DOWN",   'j' },
	{ "Move Left",        "CTRL_LEFT",   'h' },
	{ "Move Right",       "CTRL_RIGHT",  'l' },
	{ "Move Upper Left",  "CTRL_ULEFT",  'y' },
	{ "Move Upper Right", "CTRL_URIGHT", 'u' },
	{ "Move Down Left",   "CTRL_DLEFT",  'b' },
	{ "Move Down Right",  "CTRL_DRIGHT", 'n' },
	{ "Move Z Axis",      "CTRL_ZAXIS",  'z' },

	// scrolling
	{ "Center On Player", "CTRL_SCRL_CENTER", 'C',      },
	{ "Scroll Up",        "CTRL_SCRL_UP",     KEY_UP    },
	{ "Scroll Down",      "CTRL_SCRL_DOWN",   KEY_DOWN  },
	{ "Scroll Left",      "CTRL_SCRL_LEFT",   KEY_LEFT  },
	{ "Scroll Right",     "CTRL_SCRL_RIGHT",  KEY_RIGHT },

	// actions
	{ "Display Inventory", "CTRL_DISP_INV", 'i' },
	{ "Display Equipped",  "CTRL_DISP_EQP", 'E' },
	{ "Pickup Item",       "CTRL_PICKUP",   ',' },
	{ "Drop Item",         "CTRL_DROP",     '.' },
	{ "Consume Item",      "CTRL_CONSUME",  'c' },
	{ "Equip Item",        "CTRL_EQUIP",    'w' },
	{ "Throw Item",        "CTRL_THROW",    't' },

	// miscellaneous
	{ "Skip Turn",        "CTRL_SKIP_TURN", 's' },
	{ "Display Controls", "CTRL_DISCTRL",   ' ' },
	{ "Save Controls",    "CTRL_SAVECTRL",  'S' },
	//{ "Load Controls",    "CTRL_LOADCTRL",  'L' },
	{ "Quit",             "CTRL_QUIT",      'q' },
	{ "Command Mode",     "CTRL_COMMAND",   ':' }
}; 

//controlls with config
void save_config(const char* name){
	int i;
	FILE*f;

	char* suffix=".cfg";

	strcat(name,suffix);
	config.cfg_file=name;

	f = fopen(name, "wb+");
	
	fprintf(f, "%s=%s\n","config",config.cfg_file);
	fprintf(f, "%s=%s\n","lua",config.lua_init);
	fprintf(f, "%s=%s\n","ip",config.ip);
	fprintf(f, "%s=%d\n","port",config.port);
    fprintf(f, "%s=%d\n","walls",config.forget_walls);
	fprintf(f, "%s=%d\n","show_all",config.show_all);
	fprintf(f, "%s=%d\n","all_alone",config.all_alone);
	fprintf(f, "%s=%d\n","god_mode",config.god_mode);
	fprintf(f, "%s=%d\n","real_time",config.real_time);
	fprintf(f, "%s=%d\n","real_time",config.multiplayer);
	fprintf(f, "%s=%d\n","anime_throw",config.throw_anim_delay);

	for(i=0;i<TOTAL_CONTROLS;i++){
		switch(controls[i].ctrl_CTRL){
			case KEY_UP:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_UP");
			break;
			case KEY_DOWN:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_DOWN");
			break;
			case KEY_LEFT:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_LEFT");
			break;
			case KEY_RIGHT:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_RIGHT");
			break;
			default :
				fprintf(f, "%s=%c\n",controls[i].enum_CTRL,controls[i].ctrl_CTRL);
		}
	}
	fclose(f);
}


//helper function for drawing controlls
void check_spot(int *spot,int *offset,int ysize){
	if(*spot<0){
		*spot=0;
		*offset=0;
	}
	if(*spot>=TOTAL_CONTROLS-1)*offset=abs((ysize-1)-(TOTAL_CONTROLS));
	if(*spot>=TOTAL_CONTROLS-1)*spot=TOTAL_CONTROLS-1;
}

void check_offset(int *offset,int ysize){
	if(*offset<0)*offset=0;
	if(*offset>abs((ysize)-(TOTAL_CONTROLS)))*offset=abs((ysize)-(TOTAL_CONTROLS));
}

//drawing controls with scrolling 
void draw_controls(int offset,int spot,int ysize){
	int i=0;
	int size=((ysize+offset)<TOTAL_CONTROLS)?(ysize+offset):TOTAL_CONTROLS;
	//memo("offset: %d spot: %d ysize: %d size: %d N: %d",offset,spot,ysize,size,TOTAL_CONTROLS);
	wmove(dispscr, 0, 0);
	wprintw(dispscr, "%s\n", "CONTROLLS:");
	for(i=offset;i<size;i++){
		wprintw(dispscr, "%c", i == spot ? '*' : ' ');
		switch(controls[i].ctrl_CTRL){
			case ' ':
				wprintw(dispscr, "%s = %s ",
					controls[i].user_CTRL,"Space"
				);
			break;
			case KEY_UP:
				wprintw(dispscr, "%s = %s ",
					controls[i].user_CTRL,"Up ArrowKey"
				);
			break;
			case KEY_DOWN:
				wprintw(dispscr, "%s = %s ",
					controls[i].user_CTRL,"Down ArrowKey"
				);
			break;
			case KEY_LEFT:
				wprintw(dispscr, "%s = %s ",
					controls[i].user_CTRL,"Left ArrowKey"
				);
			break;
			case KEY_RIGHT:
				wprintw(dispscr, "%s = %s ",
					controls[i].user_CTRL,"Right ArrowKey"
				);
			break;
			default :
				wprintw(dispscr, "%s = %c ",
					controls[i].user_CTRL,controls[i].ctrl_CTRL
				);
		}
		wprintw(dispscr, "\n");
	}
	wrefresh(dispscr);
}

void display_controls(){
	int spot=0;
	int num=1;
	int offset=0;
	int xsize,ysize;
	getmaxyx(dispscr,ysize,xsize);
	ysize--;
	//memo("offset: %d spot: %d ysize: %d",offset,spot,ysize); 

	wclear(dispscr);
	wclear(memoscr);
	wclear(statscr);

	statline(0,"Press the array keys to move up or down and Space to Quit");
	statline(1,"Press the key you want to change it too");
	
	draw_controls(offset,spot,ysize);

	while(num){
		int letter=wgetch(dispscr);
		switch(letter){
			case KEY_UP: case KEY_LEFT:
				//memo("offset: %d spot: %d ysize: %d controls: %d",offset,spot,ysize,TOTAL_CONTROLS); 
				mvwaddch(dispscr,1+spot,0,' '); 
				spot--; 
				check_spot(&spot,&offset,ysize); 
				mvwaddch(dispscr,1+spot,0,'*'); 
				wrefresh(dispscr);
				if(spot>=ysize-1&&offset>0){
					offset--;
					check_offset(&offset,ysize);
					draw_controls(offset,spot,ysize);
				} 
			break;
			case KEY_DOWN: case KEY_RIGHT: 
				//memo("offset: %d spot: %d ysize: %d controls: %d",offset,spot,ysize,TOTAL_CONTROLS); 
				mvwaddch(dispscr,(spot>ysize-1)?ysize:1+spot,0,' '); 
				spot++; 
				check_spot(&spot,&offset,ysize); 
				mvwaddch(dispscr,(spot>ysize-1)?ysize:1+spot,0,'*'); 
				wrefresh(dispscr); 
				if(spot>ysize-1){
					offset++;
					check_offset(&offset,ysize);
					draw_controls(offset,spot,ysize);
				}
			break;
			case '\n':continue;
			//break;
			/*
			case ' ':
				if(duplicates()){
					wclear(memoscr);
					memo("There are Duplicates");
					continue;
				} 
				num=!num; 
				break;*/
			default :
			//memo("letter: %c control: %c",letter,config.ctrl[spot]);
				if(letter==controls[CTRL_DISCTRL].ctrl_CTRL&&duplicates()){
					wclear(memoscr);
					memo("There are Duplicates %d",duplicates());
					continue;
				}else if(letter==controls[CTRL_DISCTRL].ctrl_CTRL&&!duplicates()){
					memo("There are Duplicates %d",duplicates());
					num=!num; 
				}else{
					controls[spot].ctrl_CTRL=letter;
					statline(2,"%c=%d",(char)letter,letter);
				}
				wrefresh(statscr);
				wrefresh(memoscr);
				draw_controls(offset,spot,ysize);
		}
	}
	zone_draw(PLYR.z);
	wrefresh(dispscr);
}

int duplicates(){
	int i,j,dup=0;
	for(i=0;i<TOTAL_CONTROLS;i++)
		for(j=i+1;j<TOTAL_CONTROLS;j++)
			if(controls[i].ctrl_CTRL==controls[j].ctrl_CTRL)
				dup=1;
	return dup;
}

//saving controls
void prompt_save_controls(){
	wclear(dispscr);
	wclear(memoscr);
	wclear(statscr);
	//memo("Do you want to save to folder or Player config? Press F for folder || C for config");
	//statline(0,"Do you want to save to folder or Player config?");
	//statline(1,"Press F for folder || C for config");
	int letter;
	char* str;
	int num=1;
	int spot=0;
	int fold_con;
	wmove(memoscr, 0, 0);
	/*
	while(num){
		letter=wgetch(memoscr);
		switch(letter){
			case 'F': case 'f': fold_con=0; num=!num; break; 
			case 'C': case 'c': fold_con=1; num=!num; break;
			default :
				memo("Wrong Letter");
		}
	}
	num=!num;
	*/
	memo("What is the file name?");
	statline(0,"press q to quit");
	statline(1,"What will your config/controls name be?!?!");
	while(num){
		letter=wgetch(memoscr);
		switch(letter){
			case KEY_DC: case KEY_BACKSPACE: case 127: case 8:
				spot--;
				if(spot<=0)spot=0;
				str[spot]=NULL;
				str=(char*)realloc(str,spot*sizeof(char));
				wclear(memoscr);
				wprintw(memoscr,"What is the file name? %s %d",str,strlen(str));
				wrefresh(memoscr);
			break;
			case '\n':/*
				if(fold_con==0){
					save_controls(str);
				}else{
					save_config(str);
				}*/
				save_config(str);
				num=!num;
			break;
			default :
				if(letter==controls[CTRL_QUIT].ctrl_CTRL){
					num=!num;
				}else{
					statline(2,"%c=%d",(char)letter,letter);				
					spot++;
					str=(char*)realloc(str,spot*sizeof(char));
					str[spot-1]=(char)letter;
					wclear(memoscr);
					wprintw(memoscr,"What is the file name? %s %d",str,strlen(str));
					wrefresh(memoscr);
				}
		}
	}
	free(str);
	zone_draw(PLYR.z);
	wrefresh(dispscr);
}

void save_controls(char* name){
	FILE *f;
	int i;

	char* prefix="controls/";
	char* suffix=".ctrl";

	char* str=(char*)malloc(strlen(prefix)+strlen(name)+strlen(suffix)+1);

	strcpy(str,prefix);
	strcat(str,name);
	strcat(str,suffix);

	f=fopen(str,"wb+");
	if(f==NULL){
		memo("Error opening file %s",str);
	}else{
		for(i=0;i<TOTAL_CONTROLS;i++){
			switch(controls[i].ctrl_CTRL){
			case KEY_UP:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_UP");
			break;
			case KEY_DOWN:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_DOWN");
			break;
			case KEY_LEFT:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_LEFT");
			break;
			case KEY_RIGHT:
				fprintf(f, "%s=%s\n",controls[i].enum_CTRL,"KEY_RIGHT");
			break;
			default :
				fprintf(f, "%s=%c\n",controls[i].enum_CTRL,controls[i].ctrl_CTRL);
			}
		}
		fclose(f);
	}
}
/*
//helper function for loading controls
void check_loadspot(int *spot,int *offset,int ysize,int esize){
	if(*spot<0){
		*spot=0;
		*offset=0;
	}
	if(*spot>=esize)*offset=abs((ysize-1)-(esize));
	if(*spot>=esize)*spot=esize-1;
}

void check_loadoffset(int *offset,int ysize,int esize){
	if(*offset<0)*offset=0;
	if(*offset>abs((ysize)-(esize)))*offset=abs((ysize)-(esize));
}

//loading controls
void draw_loadfiles(int offset,int spot,int ysize,dirent_name* ent_name,int esize){
	int i=0;
	int size=((ysize+offset)<esize)?(ysize+offset):esize;
	wmove(dispscr,0,0);
	wprintw(dispscr, "%s\n", "CONTROLLS FILE NAMES:");
	for(i=offset;i<size;i++){
		wprintw(dispscr, "%c", i == spot ? '*' : ' ');
		wprintw(dispscr, "%s\n",ent_name[i].name);
	}
	wrefresh(dispscr);
}

void display_loadcontrols(){
	wclear(dispscr);
	wclear(memoscr);
	wclear(statscr);

	DIR* dir = NULL;
    struct dirent* ent = NULL;

	ent_name =(dirent_name*)calloc(20,sizeof(dirent_name));

    int spot=0;
	int num=1;
	int offset=0;
	int xsize,ysize;
	getmaxyx(dispscr,ysize,xsize);
	ysize--;

    dir=opendir("controls");

    if(dir == NULL)memo("ERROR! No Controls to Load");

    int len=0;
    while((ent=readdir(dir))!=NULL){
        if(ent==NULL){
        	memo("ERROR! Could not get name of file %d",ent_name[len].size);
        	break;
        }
        if(NULL!=strstr(ent->d_name,".ctrl")){
        	ent_name[len].size++;
        	ent_name[len].name=ent->d_name;
    		len++;
    	}
    }
    closedir(dir);

	memo("LOAD FILES: %s %s %s",ent_name[0].name,ent_name[1].name,ent_name[2].name);
	draw_loadfiles(offset,spot,ysize,ent_name,len);

	while(num){
		int letter=wgetch(dispscr);
		switch(letter){
			case KEY_UP: case KEY_LEFT:
				memo("offset: %d spot: %d ysize: %d controls: %d ent_name: %s",offset,spot,ysize,TOTAL_CONTROLS,ent_name[spot].name); 
				mvwaddch(dispscr,1+spot,0,' '); 
				spot--; 
				check_loadspot(&spot,&offset,ysize,len); 
				mvwaddch(dispscr,1+spot,0,'*'); 
				wrefresh(dispscr);
				if(spot>=ysize-1&&offset>0){
					offset--;
					check_loadoffset(&offset,ysize,len);
					draw_loadfiles(offset,spot,ysize,ent_name,len);
				}
			break;
			case KEY_DOWN: case KEY_RIGHT: 
				memo("offset: %d spot: %d ysize: %d controls: %d ent_name: %s",offset,spot,ysize,TOTAL_CONTROLS,ent_name[spot].name); 
				mvwaddch(dispscr,(spot>ysize-1)?ysize:1+spot,0,' '); 
				spot++; 
				check_loadspot(&spot,&offset,ysize,len); 
				mvwaddch(dispscr,(spot>ysize-1)?ysize:1+spot,0,'*'); 
				wrefresh(dispscr); 
				if(spot>ysize-1){
					offset++;
					check_loadoffset(&offset,ysize,len);
					draw_loadfiles(offset,spot,ysize,ent_name,len);
				}
			break;
			case '\n': num=!num; load_controls(ent_name[spot].name); break;
			default :
				if(letter==controls[CTRL_LOADCTRL].ctrl_CTRL||letter==controls[CTRL_QUIT].ctrl_CTRL){num=!num;}
				wrefresh(dispscr);
				wrefresh(memoscr);
		}
	}
   	free(ent_name);
   	zone_draw(PLYR.z);
   	wrefresh(dispscr);
}

//load controls file
char* get_cstring(FILE * f){
	int i=0, c;
	char str=[100];
	for (i = 0; i < 100 - 1; i++) {
		
		c = fgetc(f);
		if (c == EOF || c == 0 || (p != '\\' && (isspace(c) || c == '=')))
			break;
		if(c==' ')continue;
		str[i]=c;
	}
	str[i]=0;
	return str;
}

int igfspaces(FILE * f){
	int c;
	do{
		c = fgetc(f);
		//if(c == '#')while((c = fgetc(f)) != '\n' && c != EOF);
	}while(isspace(c));
	//ungetc(c, f);
	return c;
}


int get_fcontrol(FILE * f){
	static const struct {
		char * name;
		int ctrl;
	} special[] = {
		{ "up",    KEY_UP    },
		{ "down",  KEY_DOWN  },
		{ "left",  KEY_LEFT  },
		{ "right", KEY_RIGHT },
		{ "space", ' '       },
	};

	int ctrl, i;
	char * s, * o;

	o = s = get_string(f);

	for (i = 0; i < sizeof(special) / sizeof(*special); i++) {
		if (!strcmp(special[i].name, s)) {
			ctrl = special[i].ctrl;
			goto done;
		}
	}

	if (isdigit(*s))
		ctrl = atoi(s);
	else
		ctrl = *s;

done:
	free(o);
	return ctrl;
}


void load_controls(char* name){
	FILE* f;
	int i;
	char* prefix="controls/";

	char* str=(char*)malloc(strlen(prefix)+strlen(name)+1);
	strcpy(str,prefix);
	strcat(str,name);

	f = fopen(str,"rb");
	if(f==NULL){
		memo("Error opening file %s",str);
	}else{
		char* str1=NULL;
		char* str2=NULL;
		do{
			str1=get_cstring(f);
			str2=get_cstring(f);
			for(i=0;i<TOTAL_CONTROLS;i++)
				if(strcmp(str1,controls[i].enum_CTRL)==0)controls[i].ctrl_CTRL=atoi(str2);
		}while(str1!=NULL);
		free(str1);
		free(str2);
	}
	fclose(f);
}*/
