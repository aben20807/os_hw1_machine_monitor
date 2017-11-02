#ifndef HW1_PROTCL_H
#define HW1_PROTCL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MALLOC(p, s) \
        if(!((p) = malloc(s))){ \
            fprintf(stderr, "insufficient memory"); \
            exit(EXIT_FAILURE); \
        }

struct monitor_protocol {
// write someting here...
};

#endif
