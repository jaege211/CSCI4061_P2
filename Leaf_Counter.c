/*login: jaege211, x500_2
date: 03/09/18
name: Jason Jaeger, full_name2
id: 5129479, id_for_second_name*/

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
#include <ctype.h>
#include "util.h"

#define MAX_BUF 1024

typedef struct person {
	char s[1024];
	int count;
} person_t;

void removeSpaces(char *s) {
	int ptr;
	for (ptr = 0; s[ptr]; ptr++) {
		if (s[ptr] == ' ') s[ptr] = '_';
	}
	trimwhitespace(s);
}

void addString(person_t *strings, char *s, int *n) {
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
		sprintf(resultsPath, "%.*s.txt", pos, pathname + pos + 1);
	} else {
		sprintf(resultsPath, "%s.txt", pathname);
	}

	FILE *resultsFile = fopen(resultsPath, "w+");
	FILE *votesFile = fopen("votes.txt", "r");

	char output[MAX_BUF] = "";
	int count[256] = {0};
	int c, ptr;
	char line[MAX_BUF][MAX_BUF];
	person_t strings[MAX_BUF];
	char cwd[MAX_BUF];
	getcwd(cwd, MAX_BUF);

	c = 0;
	ptr = 0;
	if (votesFile != NULL && resultsFile != NULL) {

		while (fgets(line[ptr], MAX_BUF, votesFile)) {
			line[ptr][strlen(line[ptr])-1] = '\0';
			removeSpaces(line[ptr]);
			addString(strings, line[ptr], &c);
			ptr++;
		}

		fclose(votesFile);

		for (ptr = 0; ptr < 256; ptr++) {
			char candCount[MAX_BUF];
			if (strings[ptr].count > 0) {
				sprintf(candCount, "%s:%d,", strings[ptr].s, strings[ptr].count);
				strcat(output, candCount);
			}
		}

		for (ptr = 0; output[ptr+1] != '\0' || ptr >= MAX_BUF; ptr++);
	
		output[ptr] = '\n';
		fputs(output, resultsFile);
		fclose(resultsFile);

		char cwd[MAX_BUF];
		getcwd(cwd, MAX_BUF);
		printf("%s/%s\n", cwd, resultsPath);

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
