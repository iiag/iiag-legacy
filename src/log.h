//
// log.h
//

#ifndef LOG_H
#define LOG_H

typedef enum {
  LOG_ALL,      //Don't write with this value; don't filter.
  LOG_DEBUG,    //Write/filter events only interesting to developers.
  LOG_INFO,     //Write/filter events that are business as usual.
  LOG_NOTICE,   //Write/filter events that are worthy of note, but not likely to be erroneous.
  LOG_WARNING,  //Write/filter events that are unusual and possibly erroneous.
  LOG_ERROR     //Write/filter events that are erroneous.
} log_level_t;

void wrlog(log_level_t, const char *, ...);
#define debug(msg, ...) wrlog(LOG_DEBUG, msg, ##__VA_ARGS__)
#define info(msg, ...) wrlog(LOG_INFO, msg, ##__VA_ARGS__)
#define notice(msg, ...) wrlog(LOG_NOTICE, msg, ##__VA_ARGS__)
#define warning(msg, ...) wrlog(LOG_WARNING, msg, ##__VA_ARGS__)
#define error(msg, ...) wrlog(LOG_ERROR, msg, ##__VA_ARGS__)

void start_timer(void);
void end_timer(const char *);

extern const char * log_file;

#endif
