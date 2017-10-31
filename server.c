#include "server.h"

int main(int argc, char **argv)
{
	FILE *fin;
	fin = open_file("t.c");
	fclose(fin);
	pid_t pid = 1;
	fin = open_file(create_status_path(pid));
	char line_buffer[BUFSIZ]; /* BUFSIZ is defined if you include stdio.h */
	int line_number = 0;
	while (fgets(line_buffer, sizeof(line_buffer), fin)) {
		printf("%4d: %s", line_number, line_buffer);
		line_number++;
	}
	fclose(fin);
	printf("\nexit\n");
	return 0;
}

FILE* open_file(const char *fileName)
{
	FILE *fin;
	if (!(fin = fopen(fileName, "r"))) {
		printf("No file!\n");
		exit(1);
	} else {
		return fin;
	}
}

char* create_status_path(const pid_t pid)
{
	char *path = (char *) malloc(sizeof(char) * 40);
	snprintf(path, 40, "/proc/%d/status", pid);
	return path;
}
