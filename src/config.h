//
// config.h
//

#ifndef CONFIG_H
#define CONFIG_H

#include "log.h"

// defines how all the configuration data is stored
typedef struct config {
	const char * cfg_file;
	const char * sol_init;
	const char * log_file;
	const char * tileset_file;
	const char * ip;
	int port;
	int forget_walls;
	int show_all;
	int all_alone;
	int god_mode;
	int real_time;
	int multiplayer;
	log_level_t log_level;
	int log_stderr;
	int throw_anim_delay;
} config_t;

extern config_t config;

void init_config(int, char **);
void save_config(const char* pname);

#endif
