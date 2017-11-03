#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

#define PATH_SIZE 40
#define KEY_SIZE 30
#define VALUE_SIZE 30
#define ERRMSG_SIZE 30
#define PROC_NUM 300
#define LIST_CHAR_LENGTH 1000

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
int *scan_digital_dir(const char *path);
int create_server(const int port);
void accept_client(const int sockfd);
void *connection_handler(void *client_sockfd);

char *get_process_info(char command);
char *get_list_all_process_ids();
char *get_thread_s_ids();
char *get_child_s_pids();
char *get_process_name();
char *get_state_of_process();
char *get_cmdline();
char *get_parent_s_pid();
char *get_all_ancients_of_pids();
char *get_virtual_memory_size();
char *get_physical_memory_size();

#endif
