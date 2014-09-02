//
// player.h
//

#ifndef PLAYER_H
#define PLAYER_H

#include "world.h"
#include "creature.h"

#define PLYR world.plyr
#ifdef SERVER
	#define plyr_is_crtr(C) (C->ai == 0)
#else
	#define plyr_is_crtr(C) ((C) == &PLYR)
#endif

void update_vis(void);

// player willed actions
void plyr_act_pickup(int, char **);
void plyr_act_drop(int, char **);
void plyr_act_inv(int, char **);
void plyr_act_equipped(int, char **);
void plyr_act_consume(int, char **);
void plyr_act_equip(int, char **);
void plyr_act_throw(int, char **);
void plyr_act_idle(int, char **);

// player displacement
void plyr_act_move_left(int, char **);
void plyr_act_move_right(int, char **);
void plyr_act_move_up(int, char **);
void plyr_act_move_down(int, char **);
void plyr_act_move_upleft(int, char **);
void plyr_act_move_upright(int, char **);
void plyr_act_move_downleft(int, char **);
void plyr_act_move_downright(int, char **);
void plyr_act_enter(int, char **);

// the inevitable
void plyr_ev_birth(void);
void plyr_ev_death(creature *, const char *);
void plyr_ev_lvlup(creature *);
void plyr_ev_act_comp(creature *, item *);
void plyr_ev_act_fail(creature *, void *);

#endif
