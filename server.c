#include "server.h"

int main(int argc, char **argv)
{
	FILE *f;
	open_file(&f, "t.c");
	return 0;
}

void open_file(FILE **fin, const char *fileName)
{
	if (!(*fin = fopen(fileName, "r"))) {
		printf("No file!\n");
		exit(1);
	}
}
