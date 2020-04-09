# -*- MakeFile -*-

#target: dependencies
#	action

all: v1 v2 child

v1:	ver1.c 
	gcc -o v1 ver1.c 
 
v2: ver2.c
	gcc -o v2 ver2.c
     
child:	child.c
	gcc -o child child.c

clean:
	-rm -f ver1.o
	-rm -f v1
	-rm -f ver2.o
	-rm -f v2
	-rm -f child.o
	-rm -f child
