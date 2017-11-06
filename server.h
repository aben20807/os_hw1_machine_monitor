#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

#define MAX_LISTEN_NUM 50
#define PATH_SIZE 40
#define KEY_SIZE 30
#define VALUE_SIZE 30
#define ERRMSG_SIZE 30
#define CMDLINE_SIZE 100
#define ID_WIDTH 10
#define PROC_NUM 300
#define LIST_CHAR_LENGTH 1000

#define MALLOC(p, s) \
    if(!((p) = malloc(s))){ \
        fprintf(stderr, "insufficient memory"); \
        exit(EXIT_FAILURE); \
    }
#define CALLOC(p, l, s) \
    if(!((p) = calloc(l, s))){ \
        fprintf(stderr, "insufficient memory"); \
        exit(EXIT_FAILURE); \
    }
#define FREE(p) \
    free(p); \
    p = NULL; \


typedef int tid_t;

typedef struct Element *element_ptr;
typedef element_ptr map;
struct Element {
	char *key;
	char *value;
	element_ptr next;
};

static inline int open_file(FILE** fin, const char* fileName);
static inline char *create_status_path(const pid_t pid);
static inline char *create_cmdline_path(const pid_t pid);
static inline map create_status_map(FILE *fin);
static inline void delete_map(const map m);
static inline void split_key_value(const char *line, char **key, char **value);
static inline char *search_value(const map status_map, const char* key);
static inline int *scan_all_digital_dir(const char *path);
static inline int create_server(const int port);
static inline void accept_client(const int sockfd);
static void *connection_handler(void *client_sockfd);
static inline char *convert_int_array_to_string(int *int_array);
static char *get_status_file_field(const pid_t pid, const char *field);
static inline char *get_process_info(const char command, const pid_t pid);
static inline char *get_process_description(const char command);

static inline char *get_list_all_process_ids();
static inline char *get_thread_s_ids(const pid_t pid);
static inline char *get_child_s_pids(const pid_t pid);
static inline char *get_cmdline(const pid_t pid);
static inline char *get_all_ancients_of_pids(const pid_t pid);

#endif
