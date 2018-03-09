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
#define _BSD_SOURCE

typedef struct person {
	char* name;
	int votes;
} person_t;

// add person to last index in election array
int addToElection(char* name, person_t* election)
{
	int i = 0;
	while (election[i].name != NULL) i++;

	election[i].name = name;
	election[i].votes = 0;

	return i;
}

// check if a person is in the election
int checkInElection(char* name, person_t* election)
{
	int i = 0;
	while (election[i].name != NULL)
	{
		if (!strcmp(election[i].name, name)) return 1;
		i++;
	}

	return 0;
}

// find person in election by name
int findIndexByName(char* name, person_t* election)
{
	int i = 0;
	while (election[i].name != NULL)
	{
		if (!strcmp(election[i].name, name)) return i;
		i++;
	}

	return -1;
}

// adds to the vote total of person
int addToCount(char* name, char* amount, person_t* election)
{
	int idx;
	if (checkInElection(name, election)) idx = findIndexByName(name, election);
	else idx = addToElection(name, election);

	election[idx].votes += atoi(amount);

	//printf("%s now has %d votes\n", election[idx].name, election[idx].votes);
}

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

// call Aggregate_Votes on all child directories
int execOnChildren(DIR *pDir, struct dirent *pEntry)
{
	rewinddir(pDir);

	pid_t pid[MAX_BUF];
	int status[MAX_BUF];
	int i= -1;

	while ((pEntry = readdir(pDir)) != NULL)
	{
		if (pEntry->d_type == DT_DIR && strcmp(pEntry->d_name, ".") != 0 && strcmp(pEntry->d_name, "..") != 0)
		{
			pid[++i] = fork();

			if (pid[i] == 0) { // child
				execlp("Aggregate_Votes", "Aggregate_Votes", pEntry->d_name, (char *) NULL);
			} else if (pid[i] > 0) { // parent
				waitpid(pid[i], &status[i], 0);
				close(pid[i]);
			} else { // error
				perror("Fork error");
				return 1;
			}
		}
	}

	return 0;
}

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("Incorrect number of arguments, Expected 1 but %d entered\n", argc - 1);
		return 1;
	}

	struct dirent entry;

	DIR *pDir = opendir(argv[1]);
	struct dirent *pEntry = &entry;

	if (pDir== NULL) // check if there is an error opening the directory
	{
		perror(argv[1]);
		return 1;
	}

	if (chdir(argv[1])) // check if there is an error changing directories
	{
		perror("File error");
		return 1;
	}

	if (checkIsLeaf(pDir, pEntry)) // execute leaf_counter if the directory is a leaf
	{
		chdir("..");
		closedir(pDir);
		execlp("Leaf_Counter", "Leaf_Counter", argv[1], (char *) NULL);
	}

	execOnChildren(pDir, pEntry);

	person_t election[MAX_BUF];

	FILE* childResults;
	char childResultsFileName[MAX_BUF];

	FILE* parentResults;
	char parentResultsFileName[MAX_BUF];

	int i;
	int j;
	char rawdata[MAX_BUF];
	char*** commaSeparated = (char***) malloc(sizeof(char) * MAX_BUF * MAX_BUF);
	char*** colonSeparated = (char***) malloc(sizeof(char) * MAX_BUF * MAX_BUF);

	rewinddir(pDir);

	// parese results file of all child directories
	while ((pEntry = readdir(pDir)) != NULL)
	{
		if (pEntry->d_type == DT_DIR && strcmp(pEntry->d_name, ".") != 0 && strcmp(pEntry->d_name, "..") != 0)
		{
			if (chdir(pEntry->d_name)) 
			{
				perror("Directory error");
				return 1;
			}

			sprintf(childResultsFileName, "%s.txt", pEntry->d_name);
			childResults = fopen(childResultsFileName, "r");

			if (childResults == NULL) 
			{
				perror("File error");
				return 1;
			}

			// get line of text
			fgets(rawdata, sizeof(rawdata), childResults);
			
			// separate it by commas
			makeargv(rawdata, ",", commaSeparated);

			// get data form commas
			i = 0;
			while (commaSeparated[0][i] != NULL) {
				makeargv(commaSeparated[0][i], ":", colonSeparated);

				addToCount(colonSeparated[0][0], colonSeparated[0][1], election);

				i++;
			}

			fclose(childResults);
			chdir("..");
		}
	}

	// make output file for directory

	char cwd[MAX_BUF];
	getcwd(cwd, MAX_BUF);

	i = 0;
	j = 0;
	while (cwd[i] != '\0') {
		if (cwd[i] == '/')
			j = i;
		i++;
	}

	sprintf(parentResultsFileName, "%.*s.txt", j, cwd + j + 1);
	parentResults = fopen(parentResultsFileName, "w+");

	if (parentResults == NULL)
	{
		perror("File Error");
		return 1;
	}

	i = 0;

	while (election[i].name != NULL)
	{
		fprintf(parentResults, "%s:%d", election[i].name, election[i].votes);
		if (election[i+1].name != NULL)
			fputc(',', parentResults);

		i++;
	} 

	fputc('\n', parentResults);
	fclose(parentResults);

	free(commaSeparated);
	free(colonSeparated);

	return 0;
}
