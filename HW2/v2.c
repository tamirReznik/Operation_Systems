/*
 *  Created on: Dec 28, 2019
 *      Author: Tamir Reznik
 */

#include "v2.h"

struct threadManager {
	int (*functionsArray[NUM_OF_JOB_METHODS])(void *);
	int balance[NUM_OF_JOB_METHODS];
	int testsMade;
	char *boards;
} tm;

int result = 0;

pthread_mutex_t incrementLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t getJobLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notifyMain = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[]) {
	int i;
	pthread_t threads[N_THREADS];

	tm.boards = readBoards(argc, argv); //creates the boards from file||Stdin

//	printBoard(tm.boards, argc - 1); //print the boards buffer checking that the readBoards works fine...

	initJobs();

//	createThreads:
	for (i = 0; i < N_THREADS; ++i)
		if (pthread_create(&threads[i], NULL, threadJob, NULL))
			exitProgram("Failed creating the thread!\n", 28);
//	wait to be notified by thread:
	if (pthread_cond_wait(&notifyMain, &getJobLock))
		exitProgram("Failed wait the thread!\n", 24);

	checkResult();

	joinThreads(threads);

	destroyPthreadVars();

	return 0;

}

void joinThreads(pthread_t *threads) {
	int i;
	for (i = 0; i < N_THREADS; ++i)
		if (pthread_join(threads[i], NULL))
			exitProgram("Failed join the thread!\n", 28);

}

void initJobs() {

	tm.functionsArray[0] = checkL;
	tm.functionsArray[1] = checkC;
	tm.functionsArray[2] = cLRslant;
	tm.functionsArray[3] = cRLslant;

	tm.balance[0] = 7;
	tm.balance[1] = 7;
	tm.balance[2] = 14;
	tm.balance[3] = 14;

	tm.testsMade = 0;
}

/*
 * threadJob() - ***Thread execute***
 * 		jobIndex - Used to retrieve specific job
 *		index - Used to determine which line/col/slant the thread should check
 *		res - hold for each job if valid or not (0/1)
 */
void *threadJob() {

	int jobIndex, index, res;

	while (tm.testsMade < NUM_OF_JOBS) {
		//lock - only single thread can take unique job
		if (pthread_mutex_lock(&getJobLock))
			exitProgram("Failed mutex_lock!\n", 19);

		if ((jobIndex = getJob()) <= -1) {
			if (pthread_cond_signal(&notifyMain))
				exitProgram("Failed cond_signal!\n", 20);
			if (pthread_mutex_unlock(&getJobLock))
				exitProgram("Failed mutex_unlock!\n", 21);
			goto j;
		}
		index = tm.balance[jobIndex];
		tm.balance[jobIndex]--;

		if (pthread_mutex_unlock(&getJobLock))
			exitProgram("Failed mutex_unlock!\n", 21);

		//Concurrency - each thread can take unique job in parallel to another threads
		res = tm.functionsArray[jobIndex]((void*) &(index));

		//lock - only single thread can increment global var
		if (pthread_mutex_lock(&incrementLock))
			exitProgram("Failed mutex_lock!\n", 19);

		result += res;
		tm.testsMade += 1;

		if (pthread_mutex_unlock(&incrementLock))
			exitProgram("Failed mutex_unlock!\n", 21);
		label: j: res = 0;
	}

	if (pthread_cond_signal(&notifyMain))
		exitProgram("Failed cond_signal!\n", 20);
	pthread_exit(NULL);

}

void exitProgram(char *message, int length) {

	write(STDERR_FILENO, message, length);
	exit(0);
}

int getJob() {

	int i;
	for (i = 0; i < NUM_OF_JOB_METHODS; i++)
		if (tm.balance[i] >= 0)
			return i;

	return -1;
}

void checkResult() {
	if (result == NUM_OF_JOBS)
		printf("\nSolution is legal\n");
	else
		printf("\nSolution is not legal\n");
}

void destroyPthreadVars() {
	if (pthread_mutex_destroy(&incrementLock)
			|| pthread_mutex_destroy(&getJobLock)
			|| pthread_cond_destroy(&notifyMain))
		exitProgram("Failed destroy Pthreads variables\n", 34);

}

int checkL(void *arg) {

	int index = *(int *) arg;
	int count = 0;

	for (int j = 0; j < 8; ++j)
		if ((tm.boards)[index * 8 + j] == 'Q') {
			count++;
			if (count > 1)
				return 0;
		}

	if (count == 0)
		return 0;

	return 1;
}

int checkC(void *arg) {

	int index = *(int *) arg;
	int count = 0;

	for (int j = 0; j < 8; ++j)
		if ((tm.boards)[j * 8 + index] == 'Q') {
			count++;
			if (count > 1)
				return 0;
		}

	if (count == 0)
		return 0;

	return 1;
}

int cLRslant(void *arg) {

	int index = *(int *) arg;
	int count;

	if (index < 8) {

		count = 0;
		for (int j = 0; j < 8 && j * 8 + j + (7 - index) < 64 - (7 - index) * 7;
				++j) {

			if ((tm.boards)[j * 8 + j + (7 - index)] == 'Q') {

				count++;

				if (count > 1)
					return 0;

			}
		}

	} else {
		count = 0;
		for (int j = 0; j < 8 && (15 - index) * 8 + j * 9 < 64 - (15 - index);
				++j) {
//			printf(" %c ", (*tm.boards)[(15 - index) * 8 + j * 9]);
			if ((tm.boards)[(15 - index) * 8 + j * 9] == 'Q') {
				count++;

				if (count > 1)
					return 0;
			}

		}
//		printf("\n");
	}

	return 1;
}

int cRLslant(void *arg) {

	int index = *(int *) arg;
	int count;
//	printf("\nright to left up to down:\n\n");
	if (index < 7) {

		count = 0;
		for (int j = 0; j < 8 && index + 7 * j <= index * 8; ++j) {
//			printf(" %c ", (*tm.boards)[index + 7 * j]);
			if (tm.boards[index + 7 * j] == 'Q') {
				count++;

				if (count > 1)
					return 0;

			}

		}
//		printf("\n");

	} else {

		count = 0;
		for (int j = 0;
				j < 8
						&& (j + 1 + (14 - index)) * 7 + (14 - index)
								<= 8 * 7 + (14 - index); ++j) {
			if (tm.boards[(j + 1 + (14 - index)) * 7 + (14 - index)] == 'Q') {
				count++;

				if (count > 1)
					return 0;

			}

		}
	}

	return 1;
}

//read board from input using readFrom method
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

