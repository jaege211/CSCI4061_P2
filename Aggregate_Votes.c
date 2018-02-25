#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef MAX_BUF
#define MAX_BUF 1024
#endif


// check if directory is a leaf
int checkIsLeaf(DIR *pDir, struct dirent *pEntry, struct stat *pStats)
{
	rewinddir(pDir);

	readdir(pDir); // ..
	readdir(pDir); // .

	while ((pEntry = readdir(pDir)) != NULL) {
		if (S_ISDIR(pStats->st_mode)) 
			return 0;
	}
	
	return 1;
}

int execOnChildren(DIR *pDir, struct dirent *pEntry, struct stat *pStats, const char *testCasePath) 
{
	rewinddir(pDir);

	readdir(pDir); // ..
	readdir(pDir); // .

	pid_t pid[MAX_BUF];
	int status[MAX_BUF];
	int i= -1;

	char *subDirPath = (char *) malloc(sizeof(char *) * MAX_BUF);
	char *curDir = (char *) malloc(sizeof(char *) * MAX_BUF);
	char *agPath = (char *) malloc(sizeof(char *) * MAX_BUF);

	getcwd(curDir, MAX_BUF);
	sprintf(agPath, "%s/%s", curDir, "Aggregate_Votes");

	while ((pEntry = readdir(pDir)) != NULL) {
		if (S_ISDIR(pStats->st_mode)) {
			sprintf(subDirPath, "%s/%s", testCasePath, pEntry->d_name);
			if (chdir(subDirPath)) {
				perror(subDirPath);
				return 1;
			}

			pid[++i] = fork();

			if (pid[i] == 0) { // child
				execl(agPath, "./Aggregate_Votes", subDirPath, (char *) NULL);
			} else if (pid[i] > 0) { // parent waits
				waitpid(pid[i], &status[i], 0);
				printf("Child: %d\nStatus: %d\nSubdirectory: %s\n\n", pid[i], status[i], subDirPath);
			} else { // error
				perror("Fork error");
				return 1;
			}

			chdir("..");
		}
	}

	return 1;
}


int main(int argc, char const *argv[])
{
	if (argc != 2) {
		printf("Incorrect number of arguments, Expected 1 but %d entered\n", argc - 1);
		return 0;
	}

	execlp("cat", "hello", (char *) NULL);

	struct stat stats;
	struct dirent entry;

	DIR *pDir = opendir(argv[1]);
	struct dirent *pEntry = &entry;
	struct stat *pStats = &stats;

	if (pDir== NULL) { 
		char *curDir = (char *) malloc(sizeof(char *) * MAX_BUF);
		getcwd(curDir, MAX_BUF);

		printf("%s\n", curDir);
		perror("Directory error");
		return 1;
	}

	if (stat(argv[1], pStats) == -1) {
		perror("Stats error");
		return 1;
	}

	// check if this is a leaf dir
	if (checkIsLeaf(pDir, pEntry, pStats)) {
		closedir(pDir);
		execl("./Leaf_Counter", "./Leaf_Counter", argv[1], (char *) NULL);
	}

	execOnChildren(pDir, pEntry, pStats, argv[1]);

	closedir(pDir);

	return 0;
}