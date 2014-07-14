//
// config.c
//

#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h> // for KEY_UP, KEY_DOWN, etc.
#include "log.h"
#include "config.h"

///// Default Configuration /////
config_t config = {
	NULL,
	"script/init.lua",
	0, 0, 0,
	20,

	{
		// movement controls
		'k', // up
		'j', // down
		'h', // left
		'l', // right
		'y', // up left
		'u', // up right
		'b', // down left
		'n', // down right

		// scrolling controls
		'C',
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,

		// action controls
		'i', // display inventory
		'E', // display equipped
		',', // pickup
		'.', // drop
		'c', // consume
		'w', // equip
		't', // throw

		// miscellaneous controls
		' ', // skip turn
		'q', // quit
	},
};

///// Define the Configuration Language /////
struct field {
	enum {
		STRING,
		CONTROL,
		BOOLEAN,
		INTEGER,
	} type;

	char * name;
	void * ptr;
};

static const struct field cfg_fields[] = {
	{ STRING,  "lua-init",         &config.lua_init         },
	{ BOOLEAN, "show-all",         &config.show_all         },
	{ BOOLEAN, "forget-walls",     &config.forget_walls     },
	{ BOOLEAN, "all-alone",        &config.all_alone        },
	{ INTEGER, "throw-anim-delay", &config.throw_anim_delay },

	// movement controls
	{ CONTROL, "ctrl-up",     config.ctrl + CTRL_UP     },
	{ CONTROL, "ctrl-down",   config.ctrl + CTRL_DOWN   },
	{ CONTROL, "ctrl-left",   config.ctrl + CTRL_LEFT   },
	{ CONTROL, "ctrl-right",  config.ctrl + CTRL_RIGHT  },
	{ CONTROL, "ctrl-uleft",  config.ctrl + CTRL_ULEFT  },
	{ CONTROL, "ctrl-uright", config.ctrl + CTRL_URIGHT },
	{ CONTROL, "ctrl-dleft",  config.ctrl + CTRL_DLEFT  },
	{ CONTROL, "ctrl-dright", config.ctrl + CTRL_DRIGHT },

	// scrolling controls
	{ CONTROL, "ctrl-scrl-center", config.ctrl + CTRL_SCRL_CENTER },
	{ CONTROL, "ctrl-scrl-up",     config.ctrl + CTRL_SCRL_UP     },
	{ CONTROL, "ctrl-scrl-down",   config.ctrl + CTRL_SCRL_DOWN   },
	{ CONTROL, "ctrl-scrl-left",   config.ctrl + CTRL_SCRL_LEFT   },
	{ CONTROL, "ctrl-scrl-right",  config.ctrl + CTRL_SCRL_RIGHT  },
};

///// Configuration file parsing /////
static int igspaces(FILE * f)
{
	int c;

	do {
		c = fgetc(f);
		if (c == '#') while ((c = fgetc(f)) != '\n' && c != EOF);
	} while (isspace(c));

	ungetc(c, f);
	return c;
}

static char * get_string(FILE * f)
{
	// this is not very elegant

#define MAX 512

	int i, c = -1, p;
	char str[MAX];
	char * dest;

	igspaces(f);

	// get delimeted string
	for (i = 0; i < MAX - 1; i++) {
		p = c;
		c = fgetc(f);

		if (c == EOF || c == 0 || (p != '\\' && (isspace(c) || c == '=')))
			break;

		str[i] = c;
	}

	str[i] = 0;
	if (c != EOF) ungetc(c, f);

	// return malloced string
	dest = malloc(strlen(str) + 1);
	strcpy(dest, str);
	return dest;

#undef MAX
}

static int get_control(FILE * f)
{
	static const struct {
		char * name;
		int ctrl;
	} special[] = {
		{ "%up%",    KEY_UP    },
		{ "%down%",  KEY_DOWN  },
		{ "%left%",  KEY_LEFT  },
		{ "%right%", KEY_RIGHT },
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

static int get_boolean(FILE * f, const char * fn)
{
	int b;
	char * s = get_string(f);

	b = !strcmp("true", s);
	if (!b && strcmp("false", s)) {
		wrlog("%s: expected 'true' or 'false' instead of '%s'", fn, s);
	}

	free(s);
	return b;
}

static int get_integer(FILE * f)
{
	char * s = get_string(f);
	int v = atoi(s);
	free(s);
	return v;
}

static void expect(char c, FILE * f, const char * fn)
{
	int g = fgetc(f);
	if (c != g) {
		wrlog("%s: Expected %c (%d), got %c (%d)", fn, c, c, g, g);
	}
}

static void load_config(const char * file)
{
	int i;
	FILE * f;
	char * name;
	const struct field * fld;

	if (!strcmp(file, "-")) {
		f = stdin;
	} else {
		f = fopen(file, "r");
		if (f == NULL) {
			wrlog("Could not open config file '%s'", file);
			return;
		}
	}

	while (igspaces(f) != EOF) {
		fld = NULL;
		name = get_string(f);

		for (i = 0; i < sizeof(cfg_fields) / sizeof(*cfg_fields); i++) {
			if (!strcmp(cfg_fields[i].name, name)) {
				fld = cfg_fields + i;
				break;
			}
		}

		igspaces(f);
		expect('=', f, file);

		if (fld == NULL) {
			wrlog("%s: Unknown field '%s'", file, name);
			free(get_string(f));
		} else {
			switch (fld->type) {
			case STRING:
				// FIXME potential memory leak
				*(char **)fld->ptr = get_string(f);
				break;
			case BOOLEAN:
				*(int *)fld->ptr = get_boolean(f, file);
				break;
			case CONTROL:
				*(int *)fld->ptr = get_control(f);
				break;
			case INTEGER:
				*(int *)fld->ptr = get_integer(f);
				break;
			}
		}

		free(name);
	}

	if (f != stdin) fclose(f);
}

///// Command line argument parsing /////
static void print_help()
{
	fprintf(stderr,
	"\noptions:\n"
	"    -c [config file]\n"
	"        Specify the configuration file to use (- for stdin).\n"
	"    -f\n"
	"        Turn on wall forgetting.\n"
	"    -h\n"
	"        Display this useful information.\n"
	"    -i [lua init file]\n"
	"        The initial lua script to run.\n"
	"    -l\n"
	"        Turn on all alone mode, for debugging purposes.\n"
	"    -s\n"
	"        Show everything.\n"
	"\n"
	);

	exit(0);
}

void init_config(int argc, char ** argv)
{
	int i;

	// handle arguments
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'h':
				print_help();
				break;
			case 'c':
				config.cfg_file = argv[++i];
				break;
			case 'i':
				config.lua_init = argv[++i];
				break;
			case 's':
				config.show_all = 1;
				break;
			case 'f':
				config.forget_walls = 1;
				break;
			case 'l':
				config.all_alone = 1;
				break;
			default:
				wrlog("Ignoring unknown flag '%s'", argv[i]);
			}
		} else {
			wrlog("Command line argument '%s' ignored.", argv[i]);
		}
	}

	// load config file if given
	if (config.cfg_file != NULL) {
		load_config(config.cfg_file);
	}
}
