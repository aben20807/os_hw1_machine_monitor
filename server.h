#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

// FILE* open_file(const char *fileName);
int open_file(FILE** fin, const char* fileName);
char* create_status_path(const pid_t pid);

#endif
