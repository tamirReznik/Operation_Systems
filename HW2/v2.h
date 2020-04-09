#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_CHARS  64
#define CHARS_IN_LINE 8
#define N_THREADS 4
#define NUM_OF_JOBS 46
#define NUM_OF_JOB_METHODS 4
//****V2 method's****//
void initJobs();
void *threadJob();
int getJob();
void joinThreads(pthread_t *threads);
void printBoard(char* board, int numOfBo);
char * readFrom(int fd, char *boards);
char* readBoards(int argc, char *argv[]);
void checkResult();
void exitProgram(char *message, int length);
void destroyPthreadVars();
//****Thread's method's****//
int checkL(void *arg);
int checkC(void *arg);
int cLRslant(void *arg);
int cRLslant(void *arg);

