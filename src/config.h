//
// config.h
//

#ifndef CONFIG_H
#define CONFIG_H

// controls enumerations
enum {
	CTRL_INVALID = -1,

	// movement
	CTRL_UP,
	CTRL_DOWN,
	CTRL_LEFT,
	CTRL_RIGHT,
	CTRL_ULEFT,
	CTRL_URIGHT,
	CTRL_DLEFT,
	CTRL_DRIGHT,
	CTRL_ENTER,

	// scrolling
	CTRL_SCRL_CENTER,
	CTRL_SCRL_UP,
	CTRL_SCRL_DOWN,
	CTRL_SCRL_LEFT,
	CTRL_SCRL_RIGHT,

	// actions
	CTRL_DISP_INV,
	CTRL_DISP_EQP,
	CTRL_PICKUP,
	CTRL_DROP,
	CTRL_CONSUME,
	CTRL_EQUIP,
	CTRL_THROW,

	// miscellaneous
	CTRL_SKIP_TURN,
	CTRL_QUIT,
	CTRL_COMMAND,

	TOTAL_CONTROLS
};

// defines how all the configuration data is stored
typedef struct config {
	const char * cfg_file;
	const char * lua_init;
	const char * ip;
	int port;
	int forget_walls;
	int show_all;
	int all_alone;
	int god_mode;
	int real_time;
	int multiplayer;
	int throw_anim_delay;
	int ctrl[TOTAL_CONTROLS];
} config_t;

extern config_t config;

void init_config(int, char **);
void save_config(void);

#endif
