#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

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

	struct stat stats;
	struct dirent entry;

	DIR *pDir = opendir(argv[1]);
	struct dirent *pEntry = &entry;
	struct stat *pStats = &stats;

	if (pDir== NULL) // check if there is an error opening the directory
	{
		perror(argv[1]);
		return 1;
	}

	if (stat(argv[1], pStats) == -1) // check if there is an error opening stats
	{
		perror(argv[1]);
		return 1;
	}

	if (chdir(argv[1])) // check if there is an error changing directories
	{
		perror(pEntry->d_name);
		return 1;
	}

	if (checkIsLeaf(pDir, pEntry)) // execute leaf_counter if the directory is a leaf
	{
		chdir("..");
		closedir(pDir);
		execlp("Leaf_Counter", "Leaf_Counter", argv[1], (char *) NULL);
	}

	execOnChildren(pDir, pEntry);

	char cwd[MAX_BUF];
	getcwd(cwd, MAX_BUF);
	printf("%s\n", cwd);

	person_t election[MAX_BUF];

	char str[] = "A:1,B:1,D:4,C:6";
	char* tok;
	tok = strtok(str, ",");
	printf("%s\n", tok);

	while ((tok = strtok(NULL, ",")) != NULL) {
		printf("%s\n", tok);
	}

	closedir(pDir);

	return 0;
}
