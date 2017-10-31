#include "server.h"

int main(int argc, char **argv)
{
	FILE *fin;
	fin = open_file("t.c");
	fclose(fin);
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
