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

#ifndef MAX_BUF
#define MAX_BUF 1024
#endif

typedef struct string {
	char s[1024];
	int count;
} string;

void removeChar(char *s, int ptr) {
	while (s[ptr]) {
		ptr++;
		s[ptr-1] = s[ptr];
	}
	s[ptr] = 0;
}

int isChar(char c) {
	if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
		return 1;
	}
	return 0;
}

void removeNonChar(char *s) {
	int ptr;
	for (ptr = 0; s[ptr]; ptr++) {
		if (!isChar(s[ptr])) {
			removeChar(s, ptr);
		}
	}
}

void addString(string *strings, char *s, int *n) {
	int ptr;
	for (ptr = 0; ptr < *n; ptr++) {
		if (strcmp(s, strings[ptr].s) == 0) {
			strings[ptr].count++;
			return;
		}
	}

	strcpy(strings[*n].s, s);
	strings[*n].count = 1;
	(*n) += 1;
}

int parseInput(char* pathname) 
{
	if (chdir(pathname))
	{
		perror(pathname);
		return 1;
	}

	char resultsPath[MAX_BUF];

	if (strchr(pathname, '/')) {
		int i = 0;
		int pos = 0;
		while (pathname[i] != '\0') {
			if (pathname[i] == '/')
				pos = i;
			i++;
		}
		sprintf(resultsPath, "./%.*s.txt", pos, pathname + pos + 1);
	} else {
		sprintf(resultsPath, "./%s.txt", pathname);
	}

	//printf("%d:%d\n", i, pos);
	//printf("%s\n", resultsPath);
	FILE *resultsFile = fopen(resultsPath, "w+");
	FILE *votesFile = fopen("./votes.txt", "r");

	char output[MAX_BUF] = "";
	int count[256] = {0};
	int c, ptr;
	char line[10][MAX_BUF];
	string strings[MAX_BUF];
	char cwd[MAX_BUF];
	getcwd(cwd, MAX_BUF);

	//printf("cwd: %s\nvotes path: %s\nresults path: %s\n\n", cwd, votesPath, resultsPath);

	c = ptr = 0;
	if (votesFile != NULL && resultsFile != NULL) {

		while (fgets(line[ptr], MAX_BUF, votesFile)) {
			line[ptr][strlen(line[ptr])-1] = '\0';
			removeNonChar(line[ptr]);
			addString(strings, line[ptr], &c);
			ptr++;
		}

		fclose(votesFile);

		for (ptr = 0; ptr < 256; ptr++) {
			char candCount[MAX_BUF];
			if (strings[ptr].count > 0) {
				snprintf(candCount, MAX_BUF, "%s:%d,", strings[ptr].s, strings[ptr].count);
				strcat(output, candCount);
			}
		}

		for (ptr = 0; output[ptr+1] != '\0' || ptr >= MAX_BUF; ptr++);
	
		output[ptr] = '\n';
		fputs(output, resultsFile);
		fclose(resultsFile);
		return 1;
	}

	printf("Not a leaf node.\n");

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
