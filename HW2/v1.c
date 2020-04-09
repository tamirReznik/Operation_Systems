/*
 *  Created on: Dec 28, 2019
 *      Author: Tamir Reznik
 */

#include "v1.h"

//****Shared Struct****//
struct SharedData {
	char *board;
	int results[NUM_OF_JOBS];
} sd;

int main(int argc, char *argv[]) {
	int i, num[15];

	pthread_t lThreads[8];
	pthread_t cThreads[8];
	pthread_t slrThreads[15];
	pthread_t srlThreads[15];

	sd.board = readBoards(argc, argv); //creates the boards from file||Stdin

	//	printBoard(sd.board, argc - 1); //print the boards buffer checking that the readBoards works fine...

	/*creating threads from 4 different arrays:
	 lThreads   - for checking the lines
	 cThreads   - for checking the columns
	 pthread_t  - for checking the slant's left to right starting up to down
	 srlThreads - for checking the slant's right to starting up
	 *******note: this piece of code stays in main to avoid malloc...*******
	 */
	for (i = 0; i < 15; i++) {
		num[i] = i;
		if (i < 8)
			if (pthread_create(&lThreads[i], NULL, checkL, (void*) &(num[i]))
					|| pthread_create(&cThreads[i], NULL, checkC,
							(void*) &(num[i])))
				write(STDERR_FILENO, "failed creating the thread!\n", 28);

		if (pthread_create(&slrThreads[i], NULL, cLRslant, (void*) &(num[i]))
				|| pthread_create(&srlThreads[i], NULL, cRLslant,
						(void*) &(num[i])))
			write(STDERR_FILENO, "failed creating the thread!\n", 28);
	}

	//waiting for all the threads **pthread_join**
	joinThreads(lThreads, cThreads, slrThreads, srlThreads);

	checkResult();

	return 0;

}

void joinThreads(pthread_t *lThreads, pthread_t *cThreads,
		pthread_t *slrThreads, pthread_t *srlThreads) {
	int i;
	for (i = 0; i < 15; ++i) {
		if (checkJoin(lThreads, cThreads, slrThreads, srlThreads, i))
			write(STDERR_FILENO, "failed join\n", 12);
	}
}

void checkResult() {
	int i;
	for (i = 0; i < NUM_OF_JOBS; ++i) {
		if (sd.results[i] == -1) {
			printf("\nSolution is not legal\n");
			break;
		}
	}
	if (i == NUM_OF_JOBS)
		printf("\nSolution is legal\n");
}

int checkJoin(pthread_t *lThreads, pthread_t *cThreads, pthread_t *slrThreads,
		pthread_t *srlThreads, int i) {
	if (i < 8) {
		if (pthread_join(lThreads[i], NULL) || pthread_join(cThreads[i], NULL)
				|| pthread_join(slrThreads[i], NULL)
				|| pthread_join(srlThreads[i], NULL)) {
			return 1;

		}
	} else {
		if (pthread_join(slrThreads[i], NULL)
				|| pthread_join(srlThreads[i], NULL)) {
			return 1;

		}
	}

	return 0;
}

void *checkL(void *arg) {

	int index = *(int *) arg;
	int count = 0;

	for (int j = 0; j < 8; ++j) {
//		printf("%c", (*sd.boards)[index * 8 + j]);
		if (sd.board[index * 8 + j] == 'Q') {
			count++;
			if (count > 1) {
				sd.results[index] = -1;
				pthread_exit(NULL);
			}
		}

	}
//	printf("\n");
	if (count == 0) {
		sd.results[index] = -1;

		pthread_exit(NULL);
	}

	pthread_exit(NULL);
}

void *checkC(void *arg) {

	int index = *(int *) arg;
	int count = 0;

	for (int j = 0; j < 8; ++j) {
//		printf("%c", (*sd.boards)[j * 8 + index]);
		if (sd.board[j * 8 + index] == 'Q') {
			count++;
			if (count > 1) {
				sd.results[index + 8] = -1;
				pthread_exit(NULL);
			}

		}

	}
//	printf("\n");
	if (count == 0) {
		sd.results[index + 8] = -1;

		pthread_exit(NULL);
	}

	pthread_exit(NULL);
}

void *cLRslant(void *arg) {

	int index = *(int *) arg;
	int count;

//	printf("left to right up to down:\n\n");

	if (index < 8) {

		count = 0;
		for (int j = 0; j < 8 && j * 8 + j + (7 - index) < 64 - (7 - index) * 7;
				++j) {

//			printf(" %c ", (*sd.boards)[j * 8 + j + (7 - index)]);

			if (sd.board[j * 8 + j + (7 - index)] == 'Q') {

				count++;

				if (count > 1) {
					sd.results[index + 16] = -1;
					pthread_exit(NULL);
				}

			}

		}
//		printf("\n");

	} else {
		count = 0;
		for (int j = 0; j < 8 && (15 - index) * 8 + j * 9 < 64 - (15 - index);
				++j) {
//			printf(" %c ", (*sd.boards)[(15 - index) * 8 + j * 9]);
			if (sd.board[(15 - index) * 8 + j * 9] == 'Q') {
				count++;

				if (count > 1) {
					sd.results[index + 16] = -1;
					pthread_exit(NULL);
				}
			}

		}
//		printf("\n");
	}

	pthread_exit(NULL);
}

void *cRLslant(void *arg) {

	int index = *(int *) arg;
	int count;
//	printf("\nright to left up to down:\n\n");
	if (index < 7) {

		count = 0;
		for (int j = 0; j < 8 && index + 7 * j <= index * 8; ++j) {
//			printf(" %c ", (*sd.boards)[index + 7 * j]);
			if (sd.board[index + 7 * j] == 'Q') {
				count++;

				if (count > 1) {
					sd.results[index + 31] = -1;
					pthread_exit(NULL);
				}

			}

		}
//		printf("\n");

	} else {

		count = 0;
		for (int j = 0;
				j < 8
						&& (j + 1 + (14 - index)) * 7 + (14 - index)
								<= 8 * 7 + (14 - index); ++j) {
//			printf(" %c ",(*sd.boards)[(j + 1 + (14 - index)) * 7 + (14 - index)]);
			if (sd.board[(j + 1 + (14 - index)) * 7 + (14 - index)] == 'Q') {
				count++;

				if (count > 1) {
					sd.results[index + 31] = -1;
					pthread_exit(NULL);
				}

			}

		}
//		printf("\n");

	}

	pthread_exit(NULL);
}

void exitProgram(char *message, int length) {

	write(STDERR_FILENO, message, length);
	exit(0);
}

char* readBoards(int argc, char *argv[]) {
	int fd, i;
	char *boards;

	if (argc > 1) {
		fd = open(*(argv + 1), O_RDONLY);
		if (fd == -1)
			exitProgram("open file", 10);

		boards = readFrom(fd, boards);
		if (close(fd) == -1)
			exitProgram("\nexit on close fd\n", 18);
	} else
		boards = readFrom(STDIN_FILENO, boards);

	return boards;

}

char * readFrom(int fd, char *boards) {

	char buf[1000];
	int k = 0, check, i = 0;
	boards = (char*) malloc(NUM_OF_CHARS * sizeof(char));

	if (boards == NULL)
		exitProgram("failed mallock\n", 15);

	check = read(fd, buf, 999);

	if (check == -1)
		exitProgram("failed Read Board\n", 18);

	while (k < 64 && i < check) {

		if (buf[i] == '.' || buf[i] == 'Q') {
			boards[k] = buf[i];
			k++;
		}
		i++;
	}

	if (k != 64)
		exitProgram("\nnot enough characters in the input\n", 36);

	return boards;
}

void printBoard(char* board, int numOfBo) {
	printf("\n");
	int i, j, k;
	if (!numOfBo)
		numOfBo = 1;

	for (i = 0; i < CHARS_IN_LINE; ++i) {
		for (j = 0; j < CHARS_IN_LINE; ++j)
			printf("%c ", board[i * CHARS_IN_LINE + j]);

		printf("\n");
	}
	printf("\n\n\n\n");

}
