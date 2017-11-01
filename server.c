#include "server.h"

int main(int argc, char **argv)
{
	FILE *fin;
	open_file(&fin, "t.c");
	fclose(fin);
	for (int i = 0; i < 10; i++) {
		pid_t pid = i;
		if (!open_file(&fin, create_status_path(pid))) {
			continue;
		}
		printf("\npid: %d\n", i);
		char line_buffer[BUFSIZ]; // BUFSIZ is defined in stdio.h
		int line_number = 0;
		while (fgets(line_buffer, sizeof(line_buffer), fin)) {
			printf("%4d: %s", line_number, line_buffer);
			line_number++;
		}
		printf("line: %d\n", line_number);
		fclose(fin);
	}
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

char* create_status_path(const pid_t pid)
{
	char *path = (char *) malloc(sizeof(char) * 40);
	snprintf(path, 40, "/proc/%d/status", pid);
	return path;
}
