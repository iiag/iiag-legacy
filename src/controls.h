//
// controls.h
//

#ifndef CONTROLS_H
#define CONTROLS_H

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

	// stancing
	CTRL_STANCE_NEUTRAL,
	CTRL_STANCE_DEFENSE,
	CTRL_STANCE_ATTACK,

	// miscellaneous
	CTRL_SKIP_TURN,
	CTRL_DISCTRL,
	CTRL_QUIT,
	CTRL_COMMAND,

	TOTAL_CONTROLS
};

typedef struct {
	int key;
	char * field;
	char * pretty;
	void (* func)(int, const char **);
} control_t;

extern control_t controls[TOTAL_CONTROLS];

int ctrl_by_field(const char * name);
int ctrl_by_key(int);
int execute(int ctrl, int argc, const char ** argv);
int str_command(const char * string);
int key_command(int key);

#endif

