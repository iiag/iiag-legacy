//
// log.h
//

#ifndef LOG_H
#define LOG_H

void wrlog(const char *, ...);
void start_timer(void);
void end_timer(const char *);

extern const char * log_file;

#endif
