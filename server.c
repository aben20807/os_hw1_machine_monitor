#include "server.h"

int main(int argc, char **argv)
{
	FILE *fin;
	for (int i = 0; i < 1; i++) {
		pid_t pid = i;
		printf("\npid: %d\n", pid);
		if (!open_file(&fin, create_status_path(pid))) {
			continue;
		}
		map status_map = create_status_map(fin);
		element_ptr curr_ptr = status_map;
		while (curr_ptr != NULL) {
			// printf("%s:\t",  curr_ptr -> key);
			// printf("%s\n",  curr_ptr -> value);
			// // element_ptr tmp = curr_ptr;
			curr_ptr = curr_ptr -> next;
			// free(tmp);
		}
		printf("%s\n", search_value(status_map, "PPid"));
		printf("%s\n", search_value(status_map, "State"));
		delete_map(status_map);
	}
	pid_t *p = scan_all_processes();
	for (int i = 0; i < 4; i++) {
		printf("[%d]", p[i]);
	}
	printf("\nexit\n");
	return 0;
}

int open_file(FILE **fin, const char *file_name)
{
	if (!(*fin = fopen(file_name, "r"))) {
		perror(file_name);
		return 0;
	} else {
		return 1;
	}
}

char *create_status_path(const pid_t pid)
{
	char *path = (char *) malloc(sizeof(char) * 40);
	snprintf(path, 40, "/proc/%d/status", pid);
	return path;
}

map create_status_map(FILE *fin)
{
	map status_map = NULL;
	MALLOC(status_map, sizeof(status_map));
	element_ptr curr_ptr = status_map;
	char line_buffer[BUFSIZ]; // BUFSIZ is defined in stdio.h
	int line_number = 0;
	while (fgets(line_buffer, sizeof(line_buffer), fin)) {
		// printf("%4d: %s", line_number, line_buffer);
		char *key, *value;
		split_key_value(line_buffer, &key, &value);
		if (line_number != 0) {
			MALLOC(curr_ptr -> next, sizeof(*curr_ptr));
			curr_ptr = curr_ptr -> next;
		}
		curr_ptr -> key = key;
		curr_ptr -> value = value;
		curr_ptr -> next = NULL;
		memset(line_buffer, '\0', sizeof(line_buffer));
		line_number++;
	}
	fclose(fin);
	return status_map;
}

void delete_map(const map m)
{
	element_ptr curr_ptr = m;
	while (curr_ptr != NULL) {
		element_ptr tmp = curr_ptr;
		curr_ptr = curr_ptr -> next;
		free(tmp -> key);
		free(tmp -> value);
		free(tmp);
	}
}

void split_key_value(const char *line, char **key, char **value)
{
	*key = (char *) calloc(30, sizeof(char));
	*value = (char *) calloc(30, sizeof(char));
	bool is_colon_appear = false;
	int count = 0;
	for (int i = 0; i < strlen(line); i++) {
		if (line[i] == '\n') {
			break;
		}
		if (line[i] == ':' && !is_colon_appear) {
			is_colon_appear = true;
		} else if (!is_colon_appear) {
			(*key)[i] = line[i];
		} else {
			if (line[i] == ' ' || line[i] == '\t') {
				// ignore white space and tab
				continue;
			}
			(*value)[count++] = line[i];
		}
	}
}

char* search_value(const map status_map, const char* key)
{
	element_ptr curr_ptr = status_map;
	while (curr_ptr != NULL) {
		if (strcmp(curr_ptr -> key, key) == 0) {
			return curr_ptr -> value;
		}
		curr_ptr = curr_ptr -> next;
	}
	return "ERROR: NOT_FOUND";
}


pid_t *scan_all_processes()
{
	DIR *proc = opendir("/proc");
	struct dirent *entry;
	pid_t *pid_array = (pid_t *)calloc(300, sizeof(pid_t));
	int array_count = 0;
	pid_t pid;
	if (proc == NULL) {
		perror("opendir(/proc)");
		return NULL;
	}
	while ((entry = readdir(proc)) != NULL) {
		if (!isdigit(*entry -> d_name)) {
			continue;
		}
		pid = strtol(entry -> d_name, NULL, 10);
		pid_array[array_count++] = pid;
		printf("%d, ", pid);
	}
	closedir(proc);
	return pid_array;
}
