//
// config.c
//

#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "config.h"
#include "controls.h"
#include "io/input.h"
#include "io/display.h"

///// Default Configuration /////
config_t config = {
	NULL,               // cfg_file
	"script/init.lua",  // lua_init

	// log_file
#ifdef SERVER
	"server.log",
#else
	"iiag.log",
#endif

	// tileset_file
	// The default tileset changes based on what is supported
	// TODO unicode support should probably be detected at runtime
#ifdef WITH_NCURSES
	"tileset/nc-unicode",
#else
	"tileset/none",
#endif

	"127.0.0.1", 13699, // ip, port
	0,                  // forget_walls
	0,                  // show_all
	0,                  // all_alone
	0,                  // god_mode
	0,                  // real_time
	0,                  // multiplayer
	LOG_INFO,           // log_level

	// log_stderr
	// The server by default also logs to stderr, otherwise not
#ifdef SERVER
	1,
#else
	0,
#endif

	20,                 // throw_anim_delay
};

///// Define the Configuration Language /////
struct field {
	enum {
		STRING,
		BOOLEAN,
		INTEGER,
	} type;

	char * name;
	void * ptr;
};

static const struct field cfg_fields[] = {
	{ STRING,  "lua-init",         &config.lua_init         },
	{ STRING,  "server-ip",        &config.ip               },
	{ STRING,  "log-file",         &config.log_file         },
	{ STRING,  "tileset-file",     &config.tileset_file     },
	{ BOOLEAN, "show-all",         &config.show_all         },
	{ BOOLEAN, "forget-walls",     &config.forget_walls     },
	{ BOOLEAN, "all-alone",        &config.all_alone        },
	{ BOOLEAN, "god-mode",         &config.god_mode         },
	{ BOOLEAN, "real-time",        &config.real_time        },
	{ BOOLEAN, "multiplayer",      &config.multiplayer      },
	{ BOOLEAN, "log-stderr",       &config.log_stderr       },
	{ INTEGER, "log-level",        &config.log_level        },
	{ INTEGER, "throw-anim-delay", &config.throw_anim_delay },
	{ INTEGER, "port",             &config.port             },
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

static int get_boolean(FILE * f, const char * fn)
{
	int b;
	char * s = get_string(f);

	b = !strcmp("true", s);
	if (!b && strcmp("false", s)) {
		warning("%s: expected 'true' or 'false' instead of '%s'", fn, s);
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
		warning("%s: Expected %c (%d), got %c (%d)", fn, c, c, g, g);
	}
}

static void load_config(const char * file)
{
	int i, ctrl;
	FILE * f;
	char * name;
	const struct field * fld;

	// Open the config file
	if (!strcmp(file, "-")) {
		f = stdin;
	} else {
		f = fopen(file, "r");
		if (f == NULL) {
			warning("Could not open config file '%s'", file);
			return;
		}
	}

	// Parse the config file
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
			ctrl = ctrl_by_field(name);

			if (ctrl != CTRL_INVALID) {
				controls[ctrl].gr_name = get_string(f);
			} else {
				warning("%s: Unknown field '%s'", file, name);
				free(get_string(f));
			}
		} else {
			switch (fld->type) {
			case STRING:
				// FIXME potential memory leak
				*(char **)fld->ptr = get_string(f);
				break;
			case BOOLEAN:
				*(int *)fld->ptr = get_boolean(f, file);
				break;
			case INTEGER:
				*(int *)fld->ptr = get_integer(f);
				break;
			}
		}

		free(name);
	}

	// Close the config file
	if (f != stdin) fclose(f);
}

// controls with config
void save_config(const char * pname)
{
	int i;
	FILE * f;

	char * name = malloc(strlen(pname) + 5);
	char * suffix = ".cfg";

	strcpy(name, pname);
	strcat(name, suffix);
	config.cfg_file = name;

	f = fopen(name, "wb+");

	// save fields as specified in cfg_fields
	for (i = 0; i < sizeof(cfg_fields) / sizeof(*cfg_fields); i++) {
		fprintf(f, "%s=", cfg_fields[i].name);

		switch (cfg_fields[i].type) {
		case STRING:
			fprintf(f, "%s\n", *(char **)cfg_fields[i].ptr);
			break;
		case BOOLEAN:
			if (*(int *)cfg_fields[i].ptr) {
				fputs("true\n", f);
			} else {
				fputs("false\n", f);
			}
			break;
		case INTEGER:
			fprintf(f, "%d\n", *(int *)cfg_fields[i].ptr);
			break;
		}
	}

	// save the controls
	for (i = 0; i < TOTAL_CONTROLS; i++) {
		fprintf(f, "%s=%s\n", controls[i].field, name_from_key(controls[i].key));
	}

	fclose(f);
}


///// Command line argument parsing /////
static void print_help()
{
	fprintf(stderr,
	"\noptions:\n"
	"    -a\n"
	"        Turn on all alone mode, for debugging purposes.\n"
	"    -c [config file]\n"
	"        Specify the configuration file to use (- for stdin).\n"
	"    -e\n"
	"        Log errors to stderr as well as the log file.\n"
	"    -f\n"
	"        Turn on wall forgetting.\n"
	"    -g\n"
	"        Turn on god mode.\n"
	"    -h\n"
	"        Display this useful information.\n"
	"    -i [lua init file]\n"
	"        The initial lua script to run.\n"
	"    -l [log file]\n"
	"        Set the log file to use.\n"
	"    -L [log level]\n"
	"        Set the logging level.\n"
	"        Supported log levels:\n"
	"            0  All\n"
	"            1  Debug\n"
	"            2  Notices\n"
	"            3  Warnings\n"
	"            4  Errors\n"
	"    -n [optional ip address]\n"
	"        Connect to server for multiplayer.\n"
	"    -r\n"
	"        Turn on all real time mode.\n"
	"    -s\n"
	"        Show everything.\n"
	"    -t [tile set file]\n"
	"        Specifies the tile set file.\n"
	"\n"
	);

	exit(0);
}

void init_config(int argc, char ** argv)
{
	int i;

	// find just the config file name
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'c') {
				config.cfg_file = argv[++i];
			}
		}
	}

	// load config file if given
#ifdef SERVER
	load_config("server.cfg");
#else
	load_config("iiag.cfg");
#endif
	if (config.cfg_file != NULL) {
		load_config(config.cfg_file);
	}

	// handle arguments
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'h':
				print_help();
				break;
			case 'c':
				// just ignore the config file now
				i++;
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
			case 'a':
				config.all_alone = 1;
				break;
			case 'g':
				config.god_mode = 1;
				break;
			case 'r':
				config.real_time = 1;
				break;
			case 'e':
				config.log_stderr = 1;
				break;
			#ifndef SERVER
			case 'n':
				config.multiplayer = 1;
				config.real_time = 1;
				if(i +1 < argc && argv[i+1][0] != '-')
					config.ip = argv[++i];

				break;
			#endif
			case 'l':
				config.log_file = argv[++i];
				break;
			case 'L':
				config.log_level = (log_level_t) atoi(argv[++i]);
				break;
			case 't':
				config.tileset_file = argv[++i];
				break;
			default:
				warning("Ignoring unknown flag '%s'", argv[i]);
			}
		} else {
			warning("Command line argument '%s' ignored.", argv[i]);
		}
	}

	// so the controls are set correctly, this is done first
	disp_init();

	// set the controls value from the graphics mode-specific names
	for (i = 0; i < TOTAL_CONTROLS; i++) {
		if (controls[i].gr_name) {
			controls[i].key = key_from_name(controls[i].gr_name);
		}
	}
}
