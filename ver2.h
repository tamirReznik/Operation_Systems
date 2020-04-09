#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define numOfChars  64
#define charsInLine 8
#define numOfChilds 4
#define pipeArrSize 2
#define NUMBER_ONE "1"
#define STDIN_FILE_NAME "STDIN"

typedef struct {
	char solution[64];
	int status[4];

} shared;

//****V2 method's****//
char** readBoards(int argc, char *argv[]);
char * readFrom(int fd, char *boards);
void printSharedBoard(shared sharedM);
void fatherWriteToSharedM(shared **sharedM, char *board, int isLast);
int forkChildrens();
void checkL(shared *shardM);
void checkC(shared *shardM);
void cLRslant(shared *shardM);
void cRLslant(shared *shardM);
int busy(shared *sharedM);
void childJob(shared *sharedM, int i, void (*fptr)(shared *s));
void fatherJob(int argc, shared **sharedM, char **boards, char **argv);
void fatherRead(int i, int argc, shared **sharedM, char **boards,
		char *fileName);
