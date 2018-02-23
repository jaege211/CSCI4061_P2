#include <stdio.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// check if directory is a leaf
int checkIsLeaf(DIR *pDIR, struct dirent *pDirent, struct stat *pStat)
{
	struct dirent *original = pDirent;

	readdir(pDIR); // ..
	readdir(pDIR); // .

	while ((pDirent = readdir(pDIR)) != NULL) {
		if (S_ISDIR(pStat->st_mode)) {
			printf("checkIsLeaf %p\n", (void *) pDirent);
			printf("checkIsLeaf [%s]\n", pDirent->d_name);
			pDirent = original;
			return 0;
		}
	}

	pDirent = original;
	return 1;
}

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		printf("Incorrect number of arguments, Expected 1 but %d entered\n", argc - 1);
		return 0;
	}

	struct stat *pStat;
	struct dirent *pDirent;
	DIR *pDIR;

	pDIR = opendir(argv[1]);

	if (pDIR == NULL) { // TODO: find way to accepts /dirname
		perror("directory failed to open");
		return 1;
	}

	if (stat(argv[1], pStat) == -1) {
		perror("stats for directory failed to open");
		return 1;
	}

	printf("Main %p\n", (void *) pDirent);

	// check if this is a leaf dir
	if (checkIsLeaf(pDIR, pDirent, pStat)) {
		closedir(pDIR);
		execl("./Leaf_Counter", "./Leaf_Counter", argv[1], (char *) NULL);
	}

	pDirent = readdir(pDIR); // ..
	printf("Main %p\n", (void *) pDirent);
	printf("Main [%s]\n", pDirent->d_name);
	pDirent = readdir(pDIR); // .
	printf("Main [%s]\n", pDirent->d_name);

	// not a leaf dir, execute appropriate programs on children
	while ((pDirent = readdir(pDIR)) != NULL) {
		if (S_ISDIR(pStat->st_mode)) {
			printf("Main [%s]\n", pDirent->d_name);
		}
	}


	closedir(pDIR);

	return 0;
}