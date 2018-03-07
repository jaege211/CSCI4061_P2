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

int parseInput(char* pathname) 
{
	if (chdir(pathname))
	{
		perror(pathname);
		return 1;
	}

	int i = 0;
	int pos = 0;
	while (pathname[i] != '\0')
	{
		if (pathname[i] == '/')
			pos = i;
		i++;
	}

	printf("%d:%d\n", i, pos);

	char resultsPath[MAX_BUF];
	sprintf(resultsPath, "./%.*s.txt", pos, pathname + pos + 1);
	printf("%s\n", resultsPath);
	FILE *resultsFile = fopen(resultsPath, "w+");

	FILE *votesFile = fopen("./votes.txt", "r");

	char output[MAX_BUF] = "";
	int count[256] = {0};
	int c, ptr;

	char cwd[MAX_BUF];
	getcwd(cwd, MAX_BUF);

	//printf("cwd: %s\nvotes path: %s\nresults path: %s\n\n", cwd, votesPath, resultsPath);

	if (votesFile != NULL && resultsFile != NULL) 
	{
		while ((c = fgetc(votesFile)) != EOF) 
			if (c != '\n' && ((c > 64 && c < 91) || (c > 96 && c < 123)))  
				count[c] += 1;

		fclose(votesFile);

		for (ptr = 0; ptr < 256; ptr++) 
		{
			char candCount[MAX_BUF];
			if (count[ptr] > 0) 
			{
				snprintf(candCount, MAX_BUF, "%c:%d,", ptr, count[ptr]);
				strcat(output, candCount);
			}
		}

		// get length of string
		for (ptr = 0; output[ptr+1] != '\0' || ptr >= MAX_BUF; ptr++);
		output[ptr] = '\n';

		fputs(output, resultsFile);
		
		fclose(resultsFile);

		//printf("%s/%s.txt\n", pathname, pathname);

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
