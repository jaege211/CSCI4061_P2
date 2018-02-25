#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#ifndef MAX_BUF
#define MAX_BUF 1024
#endif

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

	char *cwd = (char *) malloc(sizeof(char *) * MAX_BUF);

	while ((pEntry = readdir(pDir)) != NULL) 
	{
		if (pEntry->d_type == DT_DIR && strcmp(pEntry->d_name, ".") != 0 && strcmp(pEntry->d_name, "..") != 0) 
		{
			if (chdir(pEntry->d_name)) 
			{
				perror(pEntry->d_name);
				return 1;
			}

			pid[++i] = fork();

			if (pid[i] == 0) { // child
				chdir("..");
				execlp("Aggregate_Votes", "Aggregate_Votes", pEntry->d_name, (char *) NULL);
			} else if (pid[i] > 0) { // parent
				waitpid(pid[i], &status[i], 0);
				close(pid[i]);
			} else { // error
				perror("Fork error");
				return 1;
			}

			chdir("..");
		}
	}

	free(cwd);

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
		char *argDir = (char *) malloc(sizeof(char *) * MAX_BUF);
		getcwd(argDir, MAX_BUF);

		perror(argDir);
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
		printf("Leaf: %s\n", argv[1]);
		closedir(pDir);
		// execlp("Leaf_Counter", "Leaf_Counter", argv[1], (char *) NULL);
		return 0;
	}

	execOnChildren(pDir, pEntry);

	// Execute vote counter on outputs of Leaf_Counter
	printf("--- %s is done ---\n", argv[1]);

	closedir(pDir);

	return 0;
}
