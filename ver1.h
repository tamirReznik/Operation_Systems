#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define numOfChars  64
#define charsInLine 8
#define numOfChilds 4
#define pipeArrSize 2
#define NUMBER_ONE "1"
#define STDIN_FILE_NAME "STDIN"

//****Child method's****//
int checkL(char* str, int pfd);
int checkC(char* str, int pfd);
int cLRslant(char * str, int pfd);
int cRLslant(char * str, int pfd);

//****V1 method's****//
void printBoard(char** board, int numOfBo);
char * readFrom(int fd, char *boards);
char** readBoards(int argc, char *argv[]);
void prepExec(int i, int pipefd, int pipeResult);
void childPlay(int* pid, int* pipeC1, int *pipeC2, int *pipeC3, int *pipeC4,
		int *pipeResult);
void fatherWrite(int j, char **boards, char *boardIndex, int *pipeC1,
		int *pipeC2, int *pipeC3, int *pipeC4);
void fatherRead(int *pipeResult, char *fileName);
void closeAll(int *pipeC1, int *pipeC2, int *pipeC3, int *pipeC4,
		int *pipeResult, char**boards);
void writeToChild(int pipe, char**boards, char *boardIndex, int j, char *child);
