threads: threads.o
	gcc -o threads -pthread threads.o -lm
threads.o: threads.c
	gcc -c threads.c 

clean:
	rm threads
