//
// options.h
//

#ifndef OPTIONS_H
#define OPTIONS_H

#define OPT_SHOW_ALL 1

#define OPT(X) (options_set & X)

extern unsigned options_set;

void parse_options(int, char **);

#endif
