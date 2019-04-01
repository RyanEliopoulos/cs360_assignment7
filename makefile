threads: threads.o
	gcc -o threads -pthread threads.o -lm
threads.o: threads.c
	gcc -c threads.c 
all: threads.c
	gcc -o threads -pthread threads.c -lm
clean:
	rm threads threads.c threads.o makefile
run: threads
	./threads
