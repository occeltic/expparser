#Macros

CC = gcc

#CCFLAGS = 
#CCFLAGS = -ggdb3
#CCFLAGS = -g
CCFLAGS = -O3

#Dependency rules for non-file targets
all:
	make expparser
clean:
	rm -f *.o
clear:
	rm -f *~ \#*\# core *.gch
memcheck:
	valgrind --tool=memcheck --leak-check=full ./expparser < fullcalc.txt
backup:
	rm -r ../expressionTreesBackup2/*
	cp -r ../expressionTreesBackup1/* ../expressionTreesBackup2
	rm -r ../expressionTreesBackup1/*
	cp -r . ../expressionTreesBackup1
zipall:
	zip ~/Desktop/expressionTreesFull.zip ./*

zip:
	zip ~/Desktop/expressionTrees.zip ./makefile ./exptoken.h ./exptoken.c ./linklist.h ./linklist.c ./linkstack.h ./linkstack.c ./treenode.h ./treenode.c ./exptokentree.h ./exptokentree.c ./exptokenstack.h ./exptokenstack.c ./exptokentreestack.h ./exptokentreestack.c ./expparser.h ./expparser.c

#Dependency rules for files

exptoken.o: exptoken.c exptoken.h
	$(CC) $(CCFLAGS) -c exptoken.c
exptoken: exptoken.o
	$(CC) $(CCFLAGS) exptoken.o -o exptoken
linklist.o: linklist.c linklist.h
	$(CC) $(CCFLAGS) -c linklist.c
linklist: linklist.o
	$(CC) $(CCFLAGS) linklist.o -o linklist
linkstack.o: linkstack.c linkstack.h linklist.h
	$(CC) $(CCFLAGS) -c linkstack.c
linkstack: linkstack.o linklist.o
	$(CC) $(CCFLAGS) linkstack.o linklist.o -o linkstack
treenode.o: treenode.c treenode.h linklist.h
	$(CC) $(CCFLAGS) -c treenode.c
treenode: treenode.o linklist.o
	$(CC) $(CCFLAGS) treenode.o linklist.o -o treenode
exptokenstack.o: exptokenstack.h exptokenstack.c exptoken.h linkstack.h linklist.h
	$(CC) $(CCFLAGS) -c exptokenstack.c
exptokenstack: exptokenstack.o exptoken.o linkstack.o linklist.o
	$(CC) $(CCFLAGS) exptokenstack.o exptoken.o linkstack.o linklist.o -o exptokenstack
exptokentree.o: exptokentree.h exptokentree.c treenode.h linklist.h exptoken.h
	$(CC) $(CCFLAGS) -c exptokentree.c
exptokentree: exptokentree.o treenode.o linklist.o exptoken.o
	$(CC) $(CCFLAGS) exptokentree.o treenode.o linklist.o exptoken.o -o exptokentree
exptokentreestack.o: exptokentreestack.h exptokentreestack.c exptokentree.h linkstack.h linklist.h treenode.h exptoken.h
	$(CC) $(CCFLAGS) -c exptokentreestack.c
exptokentreestack: exptokentreestack.o exptokentree.o treenode.o linklist.o linkstack.o exptoken.o 
	$(CC) $(CCFLAGS) exptokentreestack.o exptokentree.o treenode.o linklist.o linkstack.o exptoken.o -o exptokentreestack
expparser.o: expparser.h expparser.c linklist.h exptoken.h exptokenstack.h exptokentree.h exptokentreestack.h
	$(CC) $(CCFLAGS) -c expparser.c
expparser: expparser.o linklist.o linkstack.o exptoken.o treenode.o exptokentree.o exptokenstack.o exptokentreestack.o
	$(CC) $(CCFLAGS) expparser.o linklist.o linkstack.o exptoken.o treenode.o exptokentree.o exptokenstack.o exptokentreestack.o -o expparser