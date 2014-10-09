//
// io/nogr/input.c
//

#ifdef WITH_SDL

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "input.h"
#include "display.h"
#include "../display.h"
#include "../../controls.h"
#include "../../config.h"

#undef isprint
#define isprint(c) ((c)>=32 && (c)<128)

void sdl_default_handler(SDL_Event *e)
{
    switch(e->type) {
    case SDL_WINDOWEVENT:
        switch(e->window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            tcols = e->window.data1 / twidth;
            trows = e->window.data2 / theight;
            SDL_FreeSurface(disp);
            //SDL_SetWindowDisplayMode(win, NULL);
            disp = SDL_GetWindowSurface(win);
            redraw();
            break;
        }
        break;
    }
}

int sdl_event_to_ctrl(SDL_Event *e) {
    int key;

    switch(e->type) {
    case SDL_KEYDOWN:
        key = e->key.keysym.sym;
        if(isprint(key) && e->key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) key = toupper(key);
        return key;
    case SDL_QUIT:
        return controls[CTRL_QUIT].key;
    default:
        sdl_default_handler(e);
        return CTRL_INVALID;
    }
}

const char * sdl_name_from_key(int key)
{
	return SDL_GetKeyName(key);
}

int sdl_key_from_name(const char * str)
{
	int key = SDL_GetKeyFromName(str);
	if(isprint(key) && isupper(str[0])) key = toupper(key);
	return key;
}

int sdl_get_key(void)
{
    SDL_Event e;
    int c = CTRL_INVALID;

    while(1) {
        SDL_WaitEvent(&e);
        if ((c = sdl_event_to_ctrl(&e)) != CTRL_INVALID) {
            return c;
        }
    } /* ???!!! */
}

int sdl_get_printable_key(void) {
    int c = 0;

    while(1) {
        c = sdl_get_key();
        if(isprint(c)) {
            return c;
        }
    }
}

int sdl_get_ctrl(void)
{
	SDL_Event e;
	int c = CTRL_INVALID;
	int last = CTRL_INVALID;

	e.type = SDL_USEREVENT;

	if(config.real_time) {
        while(SDL_PollEvent(&e)) {
            if((c = sdl_event_to_ctrl(&e)) != CTRL_INVALID && isprint(c)) {
                last = c;
            }
        }
        if(last == CTRL_INVALID) return controls[CTRL_SKIP_TURN].key;
        return last;
	} else {
        return sdl_get_printable_key();
	}
}

int sdl_prompt_dir(const char * prompt, int * dx, int * dy)
{
    sdl_memo(prompt);

	*dx=*dy=13;
	switch (ctrl_by_key(sdl_get_printable_key())) {
	case CTRL_LEFT:   *dx = -1; *dy = 0; break;
	case CTRL_DOWN:   *dx =  0; *dy = 1; break;
	case CTRL_UP:     *dx =  0; *dy =-1; break;
	case CTRL_RIGHT:  *dx =  1; *dy = 0; break;
	case CTRL_ULEFT:  *dx = -1; *dy =-1; break;
	case CTRL_URIGHT: *dx =  1; *dy =-1; break;
	case CTRL_DLEFT:  *dx = -1; *dy = 1; break;
	case CTRL_DRIGHT: *dx =  1; *dy = 1; break;
	case CTRL_SKIP_TURN: *dx =  0; *dy = 0; break;
	}

	return *dx!=13;
}

int sdl_prompt_inv(const char * prompt, inventory * inv, creature * c)
{
	SDL_Window *win = SDL_CreateWindow("Inventory Select", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_SHOWN);
	SDL_Surface *disp = SDL_GetWindowSurface(win), *text;
	SDL_Color WHITE = {255, 255, 255, 255};
	SDL_Rect cur = {0, 0, disp->w, disp->h};
    int i;
    char s[512];

    SDL_FillRect(disp, NULL, 0x44);

    text = TTF_RenderUTF8_Solid(font, prompt, WHITE);
    SDL_BlitSurface(text, NULL, disp, &cur);
    cur.y += text->h;
    SDL_FreeSurface(text);


    for(i = 0; i < inv->size; i++) {
        if(inv->itms[i]) {
			snprintf(s, 512, "%c) %s %s", ind2ch(i), inv->itms[i]->name, (c&&item_equipped(inv->itms[i], c))?"(equipped)":"");
			text = TTF_RenderUTF8_Solid(font, s, WHITE);
			SDL_BlitSurface(text, NULL, disp, &cur);
			cur.y += text->h;
			SDL_FreeSurface(text);
		}
    }
    SDL_UpdateWindowSurface(win);

    //XXX: This is hacked beyond all recognition.
    while(1) {
        int c = sdl_get_key();
        if(isprint(c)) {
            SDL_DestroyWindow(win);
            return ch2ind(c);
        }
    }
}

void sdl_prompt_equipped(const char * prompt, creature * c)
{
	SDL_Window *win = SDL_CreateWindow("Equipment Slots", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_SHOWN);
	SDL_Surface *disp = SDL_GetWindowSurface(win), *text;
	SDL_Color WHITE = {255, 255, 255, 255};
	SDL_Rect cur = {0, 0, disp->w, disp->h};
    int i;
    char s[512];

    SDL_FillRect(disp, NULL, 0x44);

    text = TTF_RenderUTF8_Solid(font, prompt, WHITE);
    SDL_BlitSurface(text, NULL, disp, &cur);
    cur.y += text->h;
    SDL_FreeSurface(text);

    for(i = 0; i < MAX_SLOTS; i++) {
        snprintf(s, 512, "%s: %s", slot_names[i], c->slots[i]?c->slots[i]->name:"(nothing)");
        text = TTF_RenderUTF8_Solid(font, s, WHITE);
        SDL_BlitSurface(text, NULL, disp, &cur);
        cur.y += text->h;
        SDL_FreeSurface(text);
    }
    SDL_UpdateWindowSurface(win);

    sdl_get_key();
    SDL_DestroyWindow(win);
}

char * sdl_prompt_string(const char * prompt)
{
    SDL_Window *win = SDL_CreateWindow("Equipment Slots", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_SHOWN);
	SDL_Surface *disp = SDL_GetWindowSurface(win), *text;
	SDL_Color WHITE = {255, 255, 255, 255};
	SDL_Rect cur = {0, 0, disp->w, disp->h};
	char *s = calloc(512, 1);
	int idx = 0;

	SDL_FillRect(disp, NULL, 0x44);

	text = TTF_RenderUTF8_Solid(font, prompt, WHITE);
    SDL_BlitSurface(text, NULL, disp, &cur);
    cur.y += text->h;
    SDL_FreeSurface(text);
    SDL_UpdateWindowSurface(win);

    while(1) {
        int c = sdl_get_key();
        switch(c) {
        case 10: case 13:
            SDL_DestroyWindow(win);
            return s;
        case 8:
            s[idx--] = '\0';
        default:
            if(isprint(c)) {
                s[idx++] = c;
            }
        }
        text = TTF_RenderUTF8_Solid(font, prompt, WHITE);
        SDL_BlitSurface(text, NULL, disp, &cur);
        SDL_FreeSurface(text);
        SDL_UpdateWindowSurface(win);
    }
}

#endif
