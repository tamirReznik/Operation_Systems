/*
 * ver2.c
 *
 *  Created on: Nov 30, 2019
 *      Author: Tamir Reznik
 */

#include "ver2.h"

int main(int argc, char *argv[]) {

	char **boards = readBoards(argc, argv);
	void (*functionsArray[])(shared *) = {checkL,checkC,cLRslant,cRLslant };
	shared *sharedM;
	int i;

	sharedM = (shared*) mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	i = forkChildrens();
	if (i < 4)
		childJob(sharedM, i, functionsArray[i]);

	else
		fatherJob(argc, &sharedM, boards, argv);

	return 0;
}

void fatherRead(int i, int argc, shared **sharedM, char **boards,
		char *fileName) {
	int isLast = 0, j;
	if (i == argc - 2)
		isLast = 1;

	fatherWriteToSharedM(sharedM, *(boards + i), isLast);
	printSharedBoard(**sharedM);

	while (busy(*sharedM))
		;

	for (j = 0; j < numOfChilds; ++j) {
		if ((*sharedM)->status[j] == -1) {
			printf("\n%s is not legal\n", fileName);
			break;
		}
		if (j == numOfChilds - 1)
			printf("\n%s is legal\n", fileName);

	}
}

void fatherJob(int argc, shared **sharedM, char **boards, char **argv) {
	int i;

	if (argc > 1) {

		for (i = 0; i < argc - 1; ++i)
			fatherRead(i, argc, sharedM, boards, argv[i + 1]);

	} else
		fatherRead(0, argc, sharedM, boards, STDIN_FILE_NAME);

}

void childJob(shared *sharedM, int i, void (*fptr)(shared *s)) {
	while (1) {
		while (sharedM->status[i] != 0 && sharedM->status[i] != -2)
			;
		if (sharedM->status[i] == -2) {
			fptr(sharedM);
			break;
		}

		fptr(sharedM);

	}
}

int busy(shared *sharedM) {
	if (sharedM->status[0] == 0 || sharedM->status[1] == 0
			|| sharedM->status[2] == 0 || sharedM->status[3] == 0
			|| sharedM->status[0] == -2 || sharedM->status[1] == -2
			|| sharedM->status[2] == -2 || sharedM->status[3] == -2)
		return 1;

	return 0;
}

int forkChildrens() {
	int i, pid;
	for (i = 0; i < numOfChilds; ++i) {

		if ((pid = fork()) == -1) {
			write(STDERR_FILENO, "\nfailed fork\n", 12);
			exit(0);
		}
		if (pid == 0)
			return i;

	}
	return i;

}

void fatherWriteToSharedM(shared **sharedM, char *board, int isLast) {

	int j;
	for (j = 0; j < numOfChars; ++j) {
		(*sharedM)->solution[j] = *(board + j);
	}
	for (j = 0; j < numOfChilds; ++j) {
		if (isLast)
			(*sharedM)->status[j] = -2;
		else
			(*sharedM)->status[j] = 0;
	}

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

char * readFrom(int fd, char *boards) {

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

void printSharedBoard(shared sharedM) {
	printf("\n");
	int i, j;

	for (i = 0; i < charsInLine; ++i) {
		for (j = 0; j < charsInLine; ++j)
			printf("%c ", sharedM.solution[i * charsInLine + j]);

		printf("\n");
	}
	printf("\n");

}

void checkL(shared *shardM) {

	int count;
	for (int i = 0; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8; ++j) {
			if (shardM->solution[i * 8 + j] == 'Q') {
				count++;
				if (count > 1) {
					shardM->status[0] = -1;
					return;
				}
			}
		}
		if (count == 0) {
			shardM->status[0] = -1;
			return;
		}
	}
	shardM->status[0] = 1;
}

void checkC(shared *shardM) {

	int count;
	for (int i = 0; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8; ++j) {
			if (shardM->solution[j * 8 + i] == 'Q') {
				count++;
				if (count > 1) {
					shardM->status[1] = -1;
					return;
				}

			}

		}
		if (count == 0) {
			shardM->status[1] = -1;
			return;
		}
	}
	shardM->status[1] = 1;
}

void cLRslant(shared *shardM) {

	int count;

	for (int i = 0; i < 8; ++i) {

		count = 0;
		for (int j = 0; j < 8 && j * 8 + j + i < 64 - i * 7; ++j) {
			if (shardM->solution[j * 8 + j + i] == 'Q') {
				count++;
				if (count > 1) {
					shardM->status[2] = -1;
					return;
				}
			}

		}

	}

	for (int i = 1; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8 && i * 8 + j * 9 < 64 - i; ++j) {

			if (shardM->solution[i * 8 + j * 9] == 'Q') {
				count++;
				if (count > 1) {
					shardM->status[2] = -1;
					return;
				}
			}

		}

	}
	shardM->status[2] = 1;
}

void cRLslant(shared *shardM) {

	int count;
	for (int i = 0; i < 7; ++i) {
		count = 0;
		for (int j = 0; j < 8 && i + 7 * j <= i * 8; ++j) {
			if (shardM->solution[i + 7 * j] == 'Q') {
				count++;
				if (count > 1) {
					shardM->status[3] = -1;
					return;
				}

			}

		}

	}

	for (int i = 0; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8 && (j + 1 + i) * 7 + i <= 8 * 7 + i; ++j) {
			if (shardM->solution[(j + 1 + i) * 7 + i] == 'Q') {
				count++;
				if (count > 1) {
					shardM->status[3] = -1;
					return;
				}
			}

		}

	}
	shardM->status[3] = 1;
}

