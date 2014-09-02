//
// log.c
//

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"
#include "introspection.h"
#include "config.h"

#ifdef SERVER
const char * log_file = "server.log";
#else
const char * log_file = "iiag.log";
#endif

static clock_t sclock;

static char * level_names[] = {
  "ALL",
  "DEBUG",
  "INFO",
  "NOTICE",
  "WARNING",
  "ERROR"
};

void wrlog(log_level_t loglevel, const char * fmt, ...)
{
	static FILE * logf = NULL;

	char * str;
	time_t tm;
	va_list vl;
	char loc[64];
	loc[63]=0;

	if(loglevel <= config.log_level) return;

	if (logf == NULL) {
		logf = fopen(log_file, "a");
		if (logf == NULL) return;
		fprintf(logf, "===============================================================================\n");
		wrlog(LOG_INFO, "Opened log file");
	}

	describe_caller(loc, 63);

	time(&tm);
	str = ctime(&tm);
	str[strlen(str) - 1] = 0;
	fprintf(logf, "%s | %-32s %-7s ", str, loc, level_names[loglevel]);

	va_start(vl, fmt);
	vfprintf(logf, fmt, vl);
	va_end(vl);

	fputc('\n', logf);
	fflush(logf);
}

void start_timer(void)
{
	sclock = clock();
}

void end_timer(const char * name)
{
	clock_t cnt = clock() - sclock;
	double sec = (double)cnt / CLOCKS_PER_SEC;
	debug("Timer %s: %g seconds (%d clocks)", name, sec, cnt);
}
