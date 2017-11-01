#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

typedef struct Element *element_ptr;
typedef element_ptr map;
struct Element {
	char *key;
	char *value;
	element_ptr next;
};

int open_file(FILE** fin, const char* fileName);
char* create_status_path(const pid_t pid);
map create_status_map(FILE *fin, const pid_t pid);
// void get_info_key(char *line, char *key, char *value);
// void split_key_value(char *line, char *key, char *value);
void split_key_value(char *line, char **key, char **value);

#endif
