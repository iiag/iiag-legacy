//
//controlls.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <dirent.h>
#include "display.h"
#include "controlls.h"
#include "../player.h"
#include "../config.h"
#include "../controls.h"

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
		switch(controls[i].key){
			case ' ':
				wprintw(dispscr, "%s = %s ",
					controls[i].pretty,"Space"
				);
			break;
			case KEY_UP:
				wprintw(dispscr, "%s = %s ",
					controls[i].pretty,"Up ArrowKey"
				);
			break;
			case KEY_DOWN:
				wprintw(dispscr, "%s = %s ",
					controls[i].pretty,"Down ArrowKey"
				);
			break;
			case KEY_LEFT:
				wprintw(dispscr, "%s = %s ",
					controls[i].pretty,"Left ArrowKey"
				);
			break;
			case KEY_RIGHT:
				wprintw(dispscr, "%s = %s ",
					controls[i].pretty,"Right ArrowKey"
				);
			break;
			default :
				wprintw(dispscr, "%s = %c ",
					controls[i].pretty,controls[i].key
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
	int xsize, ysize;
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
			default :
			//memo("letter: %c control: %c",letter,config.ctrl[spot]);
				if (letter == controls[CTRL_DISCTRL].key) {
					if(duplicates()){
						wclear(memoscr);
						memo("There are Duplicates %d",duplicates());
					}else{
						num=!num;
					}
				}else{
					controls[spot].key=letter;
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
			if(controls[i].key==controls[j].key)
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
	wmove(memoscr, 0, 0);
	memo("What is the file name?");
	statline(0,"press q to quit");
	statline(1,"What will your config/controls name be?!?!");
	while(num){
		letter=wgetch(memoscr);
		switch(letter){
			case KEY_DC: case KEY_BACKSPACE: case 127: case 8:
				spot--;
				if(spot<=0)spot=0;
				str[spot]='\0';
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
				if(letter==controls[CTRL_QUIT].key){
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

