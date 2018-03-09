/*login: jaege211, x500_2
date: 03/09/18
name: Jason Jaeger, full_name2
id: 5129479, id_for_second_name*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "util.h"

#define MAX_BUF 1024

// check if directory is a leaf
int checkIsLeaf(DIR *pDir, struct dirent *pEntry)
{
	rewinddir(pDir);

	while ((pEntry = readdir(pDir)) != NULL)
	{
		if (pEntry->d_type == DT_DIR && strcmp(pEntry->d_name, ".") != 0 && strcmp(pEntry->d_name, "..") != 0)
			return 0;
	}

	return 1;
}

// faild attempt to be good at programming... hard part is getting the name of the directory that it is linked to
char* checkLoops(FILE* pOutput, char* path)
{
	printf("%s\n", path);
	DIR* pDir = opendir(path);
	char nextDir[MAX_BUF];
	char* loop;

	struct dirent* pEntry;

	if (pDir == NULL)
	{
		perror("Directory Error");
		return NULL;
	}

	while ((pEntry = readdir(pDir)) != NULL)
	{
		if (pEntry->d_type == DT_DIR && strcmp(pEntry->d_name, ".") != 0 && strcmp(pEntry->d_name, "..") != 0)
		{
			if (pEntry->d_type == DT_LNK) return pEntry->d_name;
			if (checkIsLeaf(pDir, pEntry)) return NULL;

			loop = checkLoops(pOutput, nextDir);
			sprintf(nextDir, "%s/%s", path, pEntry->d_name);
			if (!strcmp(pEntry->d_name, loop)) fprintf(pOutput, "There is a cycle from %s to %s.\n", pEntry->d_name, loop);
			return NULL;
		}
	}

	return 0;
}

int main(int argc, char const *argv[])
{
	if (argc > 2) 
	{
		printf("Incorrect number of arguments, Expected 0 or 1 but %d were entered\n", argc - 1);
		return 1;
	}

	chdir(argv[0]);

	char cwd[MAX_BUF];
	pid_t pid = fork();


	// check input formatting
	if (pid == 0) {
		if (argc == 2) execlp("Aggregate_Votes", "Aggregate_Votes", argv[1], (char *) NULL);
		else execlp("Aggregate_Votes", "Aggregate_Votes", ".", (char *) NULL);
	} else if (pid > 0) {
		wait(NULL);

		if (chdir(argv[1])) {
			perror("Directory Error");
			return 1;
		}

		char resultsFilePath[MAX_BUF];

		if (strchr(argv[1], '/')) {
			int i = 0;
			int pos = 0;
			while (argv[1][i] != '\0') {
				if (argv[1][i] == '/')
					pos = i;
				i++;
			}
			sprintf(resultsFilePath, "./%.*s.txt", pos, argv[1] + pos + 1);
		} else {
			sprintf(resultsFilePath, "./%s.txt", argv[1]);
		}

		char cwd[MAX_BUF];
		getcwd(cwd, MAX_BUF);
		sprintf(resultsFilePath, "./%s.txt", basename(cwd));

		FILE *results = fopen(resultsFilePath, "a+");

		if (results == NULL)
		{
			perror("File Error");
			return 1;
		}

		// see which person has the most votes in the result file and append winner

		int i;
		char rawdata[MAX_BUF];
		char*** commaSeparated = (char***) malloc(sizeof(char) * MAX_BUF * MAX_BUF);
		char*** colonSeparated = (char***) malloc(sizeof(char) * MAX_BUF * MAX_BUF);
		char* winnerName;
		int winnerVotes = 0;

		// get line of text
		fgets(rawdata, sizeof(rawdata), results);
		
		makeargv(rawdata, ",", commaSeparated);

		i = 0;
		while (commaSeparated[0][i] != NULL) {
			makeargv(commaSeparated[0][i], ":", colonSeparated);

			if (atoi(colonSeparated[0][1]) > winnerVotes)
			{
				winnerVotes = atoi(colonSeparated[0][1]);
				winnerName = colonSeparated[0][0];
			}

			i++;
		}

		free(commaSeparated);
		free(colonSeparated);

		fprintf(results, "Winner: %s\n", winnerName);

		fclose(results);
	} else{
		perror("Fork Error");
	}

	return 0;
}