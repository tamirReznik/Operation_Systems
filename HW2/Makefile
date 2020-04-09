# -*- MakeFile -*-

#target: dependencies
#	action

all: v1 v2

v1:	v1.c 
	gcc v1.c -o v1 -pthread
	
v2:	v2.c 
	gcc v2.c -o v2 -pthread	
	
clean:
	-rm -f v1.o
	-rm -f v1
	-rm -f v2.o
	-rm -f v2