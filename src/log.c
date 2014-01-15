//
// log.c
//

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"

const char * log_file = "iiag.log";

void wrlog(const char * fmt, ...)
{
	static FILE * logf = NULL;

	char * str;
	time_t tm;
	va_list vl;

	if (logf == NULL) {
		logf = fopen(log_file, "a");
		if (logf == NULL) return;
		wrlog("Opened log file");
	}

	time(&tm);
	str = ctime(&tm);
	str[strlen(str) - 1] = 0;
	fprintf(logf, "%s | ", str);

	va_start(vl, fmt);
	vfprintf(logf, fmt, vl);
	va_end(vl);

	fputc('\n', logf);
	fflush(logf);
}
