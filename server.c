#include "server.h"

int main(int argc, char **argv)
{
	// FILE *fin;
	// for (int i = 0; i < 10; i++) {
	//     pid_t pid = i;
	//     printf("\npid: %d\n", pid);
	//     if (!open_file(&fin, create_status_path(pid))) {
	//         continue;
	//     }
	// }

	// pid_t pid = 1;
	// open_file(&fin, create_status_path(pid));

	char *key, *value;
	char *line = "j:    hh";
	split_key_value(line, &key, &value);
	printf("%s --> '%s' + '%s'", line, key, value);
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

map create_status_map(FILE *fin, const pid_t pid)
{
	map status_map = NULL;
	MALLOC(status_map, sizeof(status_map));
	char line_buffer[BUFSIZ]; // BUFSIZ is defined in stdio.h
	int line_number = 0;
	while (fgets(line_buffer, sizeof(line_buffer), fin)) {
		// printf("%4d: %s", line_number, line_buffer);
		line_number++;
	}
	printf("line: %d\n", line_number);
	fclose(fin);
	return status_map;
}

void split_key_value(char *line, char **key, char **value)
{
	*key = (char *) malloc(sizeof(char) * 10);
	*value = (char *) malloc(sizeof(char) * 20);
	bool is_colon_appear = false;
	int count = 0;
	for (int i = 0; i < strlen(line); i++) {
		if (line[i] == ':') {
			is_colon_appear = true;
		} else if (!is_colon_appear) {
			(*key)[i] = line[i];
		} else {
			if (line[i] == ' ') { // ignore white space
				continue;
			}
			(*value)[count++] = line[i];
		}
	}
}
