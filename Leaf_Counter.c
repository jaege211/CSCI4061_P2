#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"

int parseInput(char* pathname) {
	FILE* f, *o;
	char* path = malloc(strlen(pathname) + 256);
	char* test = (char*) malloc(sizeof(char*)*128);
	char* output = (char*) malloc(sizeof(char*)*128);
	char* temp = (char*) malloc(sizeof(char*)*1024);
	char* base = (char*) malloc(sizeof(char*)*128);
	int count[256] = {0};
	int c, ptr;

	strcpy(path,pathname);
	strcat(path, "/votes.txt");

	f = fopen(path, "rb");

	if (f != NULL) {
		while ((c = fgetc(f))) {
			if (c != EOF && c != '\n' && 
				((c > 64 && c < 91) || (c > 96 && c < 123))) {
				count[c] += 1;
				ptr++;
			} else if (c == EOF) {
				break;
			}
		}
	}

	path = pathname;
	base = basename(path);

	snprintf(test, sizeof(test)*4, "%s%s%s", "/", base, ".txt");
	strcat(path, test);
	printf("%s\n", path);

	f = fopen(path, "w+");

	for (ptr = 0; ptr < 256; ptr++) {
		if (count[ptr] > 0) {
			snprintf(output, sizeof(output)*4, "%c:%d,", ptr, count[ptr]);
			strcat(temp, output);
		}
	}

	temp[strlen(temp)-1] = 0;

	fprintf(f, "%s\n", temp);
	fclose(f);
	return 0;
}

int main(int argc, char **argv){

	if (argc != 2){
		printf("Usage: %s Program\n", argv[0]);
		return -1;
	}

	//call parseInput
	int num = parseInput(argv[1]);


	return 0;
}
