#include "server.h"

int main(int argc, char **argv)
{
	FILE *fin;
	for (int i = 2; i < 4; i++) {
		pid_t pid = i;
		printf("\npid: %d\n", pid);
		if (!open_file(&fin, create_status_path(pid))) {
			continue;
		}
		map m = create_status_map(fin);
		element_ptr curr_ptr = m;
		while (curr_ptr != NULL) {
			printf("%s:\t",  curr_ptr -> key);
			printf("%s\n",  curr_ptr -> value);
			element_ptr tmp = curr_ptr;
			curr_ptr = curr_ptr -> next;
			free(tmp);
		}
	}

	// pid_t pid = 1;
	// open_file(&fin, create_status_path(pid));
	// map m = create_status_map(fin, pid);
	// printf("%s %s", m-> key, m -> value);
	printf("\nexit\n");
	return 0;
}

int open_file(FILE **fin, const char *file_name)
{
	if (!(*fin = fopen(file_name, "r"))) {
		printf("No file: %s\n", file_name);
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
	// char *line_buffer = (char *)malloc(sizeof(char)*4096*4096); // BUFSIZ is defined in stdio.h
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

void split_key_value(char *line, char **key, char **value)
{
	*key = (char *) malloc(sizeof(char) * 30);
	*value = (char *) malloc(sizeof(char) * 30);
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
			if (line[i] == ' ' || line[i] == '\t' || \
			    line[i] == 0x007f) { // ignore white space
				continue;
			}
			// if (line[i] <= 'z' && line[i] >= 'a') { // ignore white space
			// printf("%c\n", (*value)[count++]);
			// continue;
			(*value)[count++] = line[i];
			// }
			// printf("%c\n", (*value)[count++]);
		}
	}
	//     (*key)[strlen(*key)] = '\0';
	//     (*value)[strlen(*value)] = '\0';
	//     for (int i = strlen(*key); i < 30; i++) {
	//         (*key)[i] = '\0';
	//         // printf("%c\n", (*value)[i]);
	//     }
	//     for (int i = 0; i < 30; i++) {
	//         // (*key)[i] = '\0';
	//         printf("%c", (*key)[i]);
	//     }
	//     for (int i = strlen(*value); i < 30; i++) {
	//         (*value)[i] = '\0';
	//         // printf("%c\n", (*value)[i]);
	//     }
	//     for (int i = 0; i < 30; i++) {
	//         // (*value)[i] = '\0';
	//         printf("%c", (*value)[i]);
	//     }
}
