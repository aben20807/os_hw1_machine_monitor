#ifndef HW1_PROTCL_H
#define HW1_PROTCL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DESCRIPTION_SIZE 20

#pragma pack(1)
struct monitor_protocol {
	char command;
	pid_t pid;
	char description[DESCRIPTION_SIZE];
	char info[BUFSIZ];
};
#pragma pack(0)

#endif
