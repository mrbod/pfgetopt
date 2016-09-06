#ifndef PFGETOPT_H
#define PFGETOPT_H

extern int pfoptind;
extern const char *pfoptarg;
extern int pfoptopt;

extern int pfgetopt(int argc, const char **argv, const char* optionstring);

#endif

