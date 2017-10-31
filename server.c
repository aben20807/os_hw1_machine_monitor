#include "server.h"

int main(int argc, char **argv)
{
	FILE *fin;
	fin = open_file("t.c");
	fclose(fin);
	char *c = create_status_path(1);
	printf("%s\n", c);
	fin = open_file(create_status_path(1));
	fclose(fin);
	printf("exit");
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
