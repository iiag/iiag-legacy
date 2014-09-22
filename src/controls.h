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
	CTRL_USE,

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
	CTRL_CAST,

	// stancing
	CTRL_STANCE_NEUTRAL,
	CTRL_STANCE_DEFENSE,
	CTRL_STANCE_ATTACK,

	// miscellaneous
	CTRL_SKIP_TURN,
	CTRL_DISCTRL,
	CTRL_QUIT,
	CTRL_COMMAND,
	CTRL_SELECT,
	CTRL_ZOUT,
	CTRL_ZIN,

	TOTAL_CONTROLS
};

typedef struct {
	int key;
	char * field;
	char * pretty;
	void (* func)(int, const char **);
	char * gr_name;
} control_t;

// The list of every control that is utilized in game
extern control_t controls[TOTAL_CONTROLS];

// Given a control field name, returns the control's enum value
//  returns CTRL_INVALID if there is no mapped control
int ctrl_by_field(const char * name);

// Given a control's key value, returns the control's enum value
//  returns CTRL_INVALID if there is no mapped control
int ctrl_by_key(int);

// Given a control number, and arguments, executes a command
//  Returns 1 if the control was executed, 0 if not
int execute(int ctrl, int argc, const char ** argv);

// Calls execute given a parsable command string
//  The string syntax is: command [arg0] [arg1] ...
//  returns 1 on success, 0 on failure
int str_command(const char * string);

// Calls exexcute given a key value
//  returns 1 on success, 0 on failure
int key_command(int key);

#endif

