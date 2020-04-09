#include "ver1.h"

int main(int argc, char *argv[]) {

	int pipeC1[2], pipeC2[2], pipeC3[2], pipeC4[2];
	int pipeResult[2];
	int pid = -1, j;
	char **boards;
	char isLast[1];
	char *childP[] = { "./child", NULL };

	childPlay(&pid, pipeC1, pipeC2, pipeC3, pipeC4, pipeResult); //create child via call fork 4 times+open pipes

	if (pid > 0) {

		boards = readBoards(argc, argv); //creates the boards from file||Stdin
		printBoard(boards, argc - 1); //print the boards buffer checking that the readBoards works fine...

		if (argc == 1) {

			fatherWrite(0, boards, NUMBER_ONE, pipeC1, pipeC2, pipeC3, pipeC4);
			fatherRead(pipeResult, STDIN_FILE_NAME);

		} else {
			for (j = 0; j < argc - 1; ++j) {

				if (j == argc - 2)
					isLast[0] = '1';

				fatherWrite(j, boards, isLast, pipeC1, pipeC2, pipeC3, pipeC4);
				fatherRead(pipeResult, argv[j + 1]);
			}
		}
		closeAll(pipeC1, pipeC1, pipeC1, pipeC1, pipeResult, boards);
	} else {

		dup2(pipeResult[1], STDOUT_FILENO);
		execv("./child", childP);
	}

	return 0;

}

void closeAll(int *pipeC1, int *pipeC2, int *pipeC3, int *pipeC4,
		int *pipeResult, char **boards) {
	close(pipeC1[1]);
	close(pipeC2[1]);
	close(pipeC3[1]);
	close(pipeC4[1]);
	close(pipeC1[0]);
	close(pipeC2[0]);
	close(pipeC3[0]);
	close(pipeC4[0]);
	close(pipeResult[0]);
	free(*boards);
	free(boards);
}

void fatherRead(int *pipeResult, char *fileName) {

	char result[1];
	int i, j;
	int isNotLegal = 0;

	for (j = 0; j < numOfChilds; ++j) {
		i = read(pipeResult[0], result, 1);
		if (i == -1) {
			write(STDERR_FILENO, "failed read from pipe", 21);
			exit(0);
		}
		if (result[0] != '0')
			isNotLegal = 1;

	}
	if (isNotLegal)
		printf("\n\n%s is not legal\n", fileName);
	else
		printf("\n\n%s is legal\n", fileName);

}

void fatherWrite(int j, char **boards, char *boardIndex, int *pipeC1,
		int *pipeC2, int *pipeC3, int *pipeC4) {

	char child[1];
	int i;

	for (i = 0; i < numOfChilds; ++i) {
		sprintf(child, "%d", i + 1);

		switch (i) {
		case 0:
			writeToChild(pipeC1[1], boards, boardIndex, j, child);
			break;
		case 1:
			writeToChild(pipeC2[1], boards, boardIndex, j, child);
			break;
		case 2:
			writeToChild(pipeC3[1], boards, boardIndex, j, child);

			break;
		case 3:
			writeToChild(pipeC4[1], boards, boardIndex, j, child);
			break;
		default:
			break;
		}

	}

}

void writeToChild(int pipe, char **boards, char *boardIndex, int j, char *child) {
	write(pipe, *(boards + j), 64);
	write(pipe, child, 1);
	write(pipe, boardIndex, 1);
}

char** readBoards(int argc, char *argv[]) {
	int fd, i;
	char **boards = (char**) malloc((argc - 1) * sizeof(char*));
	if (boards == NULL) {
		write(STDERR_FILENO, "malloc error", 12);
		exit(0);
	}

	if (argc > 1) {

		for (i = 0; i < argc - 1; ++i) {
			fd = open(*(argv + i + 1), O_RDONLY);
			if (fd == -1) {
				write(STDERR_FILENO, "open file", 10);
				exit(0);
			}

			*(boards + i) = readFrom(fd, *(boards + i));
			if (close(fd) == -1)
				printf("\nexit on close %d\n", i);

		}
	} else {

		*boards = readFrom(STDIN_FILENO, *boards);
	}
	return boards;

}

char* readFrom(int fd, char *boards) {

	char buf[1000];
	int k = 0, check, i = 0;
	boards = (char*) malloc(numOfChars * sizeof(char));

	if (boards == NULL) {
		write(STDERR_FILENO, "failed mallock\n", 15);
		exit(0);
	}

	check = read(fd, buf, 999);

	if (check == -1) {
		write(STDERR_FILENO, "failed Read Board\n", 18);
		exit(0);
	}

	while (k < 64 && i < check) {

		if (buf[i] == '.' || buf[i] == 'Q') {
			boards[k] = buf[i];
			k++;
		}
		i++;
	}

	if (k != 64) {
		printf("\nnot enough characters in he file\n");
		exit(0);
	}

	return boards;
}

//childPlay - fork children
void childPlay(int *pid, int *pipeC1, int *pipeC2, int *pipeC3, int *pipeC4,
		int *pipeResult) {
	int i;

	pipe(pipeResult);
	pipe(pipeC1);
	pipe(pipeC2);
	pipe(pipeC3);
	pipe(pipeC4);

	for (i = 1; i <= numOfChilds; ++i) {

		if ((*pid = fork()) < 0) {
			write(STDERR_FILENO, "\nfailed fork\n", 12);
			exit(0);
		}
		if (*pid == 0) {
			if (close(*pipeResult) == -1)
				write(STDERR_FILENO, "failed to close pipeResult\n", 27);

			break;
		}

	}

	switch (i) {
	case 1:

		if (close((pipeC1[1])) == -1)
			write(STDERR_FILENO, "failed to close pipeArray\n", 26);
		dup2(pipeC1[0], STDIN_FILENO);

		break;
	case 2:

		if (close((pipeC2[1])) == -1)
			write(STDERR_FILENO, "failed to close pipeArray\n", 26);
		dup2(pipeC2[0], STDIN_FILENO);

		break;
	case 3:

		if (close((pipeC3[1])) == -1)
			write(STDERR_FILENO, "failed to close pipeArray\n", 26);
		dup2(pipeC3[0], STDIN_FILENO);

		break;
	case 4:

		if (close((pipeC4[1])) == -1)
			write(STDERR_FILENO, "failed to close pipeArray\n", 26);
		dup2(pipeC4[0], STDIN_FILENO);

		break;
	case 5:
		if (close(*(pipeResult + 1)) == -1)
			write(STDERR_FILENO, "failed to close pipeResult\n", 27);

		if (close((pipeC1[0])) == -1 || close((pipeC2[0])) == -1
				|| close((pipeC3[0])) == -1 || close((pipeC4[0])) == -1)
			write(STDERR_FILENO, "failed to close PipeC\n", 22);

		break;
	default:

		break;
	}

}

void printBoard(char **board, int numOfBo) {
	printf("\n");
	int i, j, k;
	if (!numOfBo)
		numOfBo = 1;

	for (k = 0; k < numOfBo; ++k) {
		for (i = 0; i < charsInLine; ++i) {
			for (j = 0; j < charsInLine; ++j)
				printf("%c ", board[k][i * charsInLine + j]);

			printf("\n");
		}
		printf("\n");
	}
	printf("\n\n");

}
