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

int main(int argc, char const *argv[])
{
	if (argc > 2) 
	{
		printf("Incorrect number of arguments, Expected 0 or 1 but %d were entered\n", argc - 1);
		return 1;
	}

	chdir(argv[0]);

	if (argc == 2) execlp("Aggregate_Votes", "Aggregate_Votes", argv[1], (char *) NULL);
	else execlp("Aggregate_Votes", "Aggregate_Votes", ".", (char *) NULL);

	

	// append winner data

	return 0;
}