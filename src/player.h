//
// player.h
//

#ifndef PLAYER_H
#define PLAYER_H

#include "world.h"
#include "creature.h"

#define PLYR world.plyr
#define plyr_is_me(C) ((C) == &PLYR)

// player willed actions
void plyr_act_pickup(void);
void plyr_act_drop(void);
void plyr_act_inv(void);
void plyr_act_equipped(void);
void plyr_act_move(int, int);
void plyr_act_consume(void);
void plyr_act_equip(void);
void plyr_act_throw(void);

// the inevitable
void plyr_ev_birth(void);
void plyr_ev_death(creature *, const char *);
void plyr_ev_lvlup(void);
void plyr_ev_act_comp(creature *, item *);
void plyr_ev_act_fail(creature *, void *);

#endif
