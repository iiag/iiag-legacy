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

	char * tstr;
	time_t tm;
	va_list vl;
	char loc[64];
	loc[63]=0;

	if(loglevel < config.log_level) return;

	if (logf == NULL) {
		logf = fopen(log_file, "a");
		if (logf == NULL) return;
		fprintf(logf, "===============================================================================\n");
		wrlog(LOG_INFO, "Opened log file");
	}

	describe_caller(loc, 63);

	// get the time string
	time(&tm);
	tstr = ctime(&tm);
	tstr[strlen(tstr) - 1] = 0;

	// write to standard log file
	va_start(vl, fmt);

	fprintf(logf, "%s | %-32s %-7s ", tstr, loc, level_names[loglevel]);
	vfprintf(logf, fmt, vl);
	fputc('\n', logf);

	fflush(logf);
	va_end(vl);

	// Also write to stdout on servers
#ifdef SERVER
	va_start(vl, fmt);

	fprintf(stderr, "%s | %-32s %-7s ", tstr, loc, level_names[loglevel]);
	vfprintf(stderr, fmt, vl);
	fputc('\n', stderr);

	va_end(vl);
#endif
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
