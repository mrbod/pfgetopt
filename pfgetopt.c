#include <string.h>
#include "pfgetopt.h"

int pfoptind = 1;
const char *pfoptarg;
int pfoptopt;

static const char *opts;
static int posix;
static int nonoptions_as_1;
static int missing_arg_colon;
static int args_scanned;
static const char *arg;

static void init(const char* optionstring)
{
    opts = optionstring;
    arg = 0;
    args_scanned = 1;
    nonoptions_as_1 = 0;
    missing_arg_colon = 0;
    posix = 0;

    if(opts[0] == '+') {
        ++opts;
        posix = 1;
    } else if(opts[0] == '-') {
        ++opts;
        nonoptions_as_1 = 1;
    }

    if(opts[0] == ':') {
        missing_arg_colon = 1;
        ++opts;
    }
}

static void rotate(int argc, const char** argv, int index)
{
    const char *a = argv[index];

    for(++index; index < argc; ++index) {
        argv[index - 1] = argv[index];
    }

    argv[argc - 1] = a;
}

int pfgetopt(int argc, const char** argv, const char *optionstring)
{
    char *opt;

    pfoptarg = 0;

    if(argv == 0) return -1;

    if(pfoptind == 1) {
        if(optionstring == 0) {
            init("");
        } else {
            init(optionstring);
        }
    }

    if(!arg || (*arg == '\0')) {
        if(args_scanned >= argc) {
            /* nothing to parse */
            return -1;
        }

        arg = argv[pfoptind];

next_arg:
        if(!strchr("-/", *arg) || (*(arg + 1) == '\0')) {
            /* not an option */
            if(nonoptions_as_1) {
                pfoptarg = arg;
                arg = 0;
                ++args_scanned;
                ++pfoptind;
                return 1;
            }

            if(!posix && (args_scanned < argc)) {
                ++args_scanned;
                rotate(argc, argv, pfoptind);
                arg = argv[pfoptind];
                goto next_arg;
            }

            arg = 0;
            return -1;
        }

        /* step over option char (-/) */
        ++arg;
        ++args_scanned;
        ++pfoptind;
    }

    if(*arg == '-') {
        /* special end of options sequence, '--' */
        return -1;
    }

    opt = strchr(opts, *arg);
    if(!opt) {
        /* unknown option */
        pfoptopt = *arg;
        ++arg;
        return '?';
    }

    ++arg;

    if(opt[1] == ':') {
        /* option argument expected */
        if(*arg != '\0') {
            /* written as -Xarg */
            pfoptarg = arg;
        } else if(pfoptind < argc) {
            /* written as -X arg */
            pfoptarg = argv[pfoptind++];
            ++args_scanned;
            arg = 0;
        }
        if(pfoptarg == 0) {
            /* missing argument */
            pfoptopt = *opt;
            arg = 0;
            if(missing_arg_colon) {
                return ':';
            }
            return '?';
        }
    }

    return *opt;
}

