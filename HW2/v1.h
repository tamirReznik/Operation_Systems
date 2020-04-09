#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_CHARS  64
#define CHARS_IN_LINE 8
#define NUM_OF_JOBS 46
/*************************************************
 * Used to reach the desired index of result array->
 * ***********************************************/
#define ADD_TO_COL_INDEX 8
#define ADD_TO_SLR_INDEX 16
#define ADD_TO_SRL_INDEX 31

//****Thread's method's****//
void* checkL(void *arg);
void *checkC(void *arg);
void *cLRslant(void *arg);
void *cRLslant(void *arg);

//****V1 method's****//
void joinThreads(pthread_t *lThreads, pthread_t *cThreads,
		pthread_t *slrThreads, pthread_t *srlThreads);
void checkResult();
int checkJoin(pthread_t *lThreads, pthread_t *cThreads, pthread_t *slrThreads,
		pthread_t *srlThreads, int i);
void printBoard(char* board, int numOfBo);
char * readFrom(int fd, char *boards);
char* readBoards(int argc, char *argv[]);
void exitProgram(char *message, int length);
