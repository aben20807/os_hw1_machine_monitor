#ifndef HW1_PROTCL_H
#define HW1_PROTCL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define MALLOC(p, s) \
        if(!((p) = malloc(s))){ \
            fprintf(stderr, "insufficient memory"); \
            exit(EXIT_FAILURE); \
        }

struct monitor_protocol {
// write someting here...
};

#endif
