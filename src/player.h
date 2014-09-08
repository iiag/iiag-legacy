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
void plyr_act_pickup(int, const char **);
void plyr_act_drop(int, const char **);
void plyr_act_inv(int, const char **);
void plyr_act_equipped(int, const char **);
void plyr_act_consume(int, const char **);
void plyr_act_equip(int, const char **);
void plyr_act_throw(int, const char **);
void plyr_act_idle(int, const char **);
void plyr_stance_neutral(int, const char **);
void plyr_stance_defense(int, const char **);
void plyr_stance_attack(int, const char **);

// player displacement
void plyr_act_move_left(int, const char **);
void plyr_act_move_right(int, const char **);
void plyr_act_move_up(int, const char **);
void plyr_act_move_down(int, const char **);
void plyr_act_move_upleft(int, const char **);
void plyr_act_move_upright(int, const char **);
void plyr_act_move_downleft(int, const char **);
void plyr_act_move_downright(int, const char **);
void plyr_act_enter(int, const char **);

// the inevitable
void plyr_ev_birth(void);
void plyr_ev_death(creature *, const char *);
void plyr_ev_lvlup(creature *);
void plyr_ev_act_comp(creature *, item *);
void plyr_ev_act_fail(creature *, void *);

#endif
