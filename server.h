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
char *create_status_path(const pid_t pid);
map create_status_map(FILE *fin);
void delete_map(const map m);
void split_key_value(const char *line, char **key, char **value);
char *search_value(const map status_map, const char* key);
pid_t *scan_all_processes();
int create_server();
void accept_client(const int sockfd);

#endif
