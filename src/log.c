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
#ifdef WITH_INTROSPECTION
	#define HEADER(F) fprintf(F, "%s | %-32s %-7s ", tstr, loc, level_names[loglevel])
#else
	#define HEADER(F) fprintf(F, "%s | %-7s ", tstr, level_names[loglevel])
#endif

	static FILE * logf = NULL;

	char * tstr;
	time_t tm;
	va_list vl;

	if(loglevel < config.log_level) return;

	if (logf == NULL) {
		logf = fopen(config.log_file, "a");
		if (logf == NULL) return; // TODO do something more useful here?

		fprintf(logf, "===============================================================================\n");
		wrlog(LOG_INFO, "Opened log file");
	}

#ifdef WITH_INTROSPECTION
	char loc[64];
	describe_caller(loc, 63);
#endif

	// get the time string
	time(&tm);
	tstr = ctime(&tm);
	tstr[strlen(tstr) - 1] = 0;

	// write to standard log file
	va_start(vl, fmt);

	HEADER(logf);
	vfprintf(logf, fmt, vl);
	fputc('\n', logf);

	fflush(logf);
	va_end(vl);

	// possibly also write to stderr
	if (config.log_stderr) {
		va_start(vl, fmt);

		HEADER(stderr);
		vfprintf(stderr, fmt, vl);
		fputc('\n', stderr);

		va_end(vl);
	}
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
