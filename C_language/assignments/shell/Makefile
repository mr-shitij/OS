try: compile execute
compile: main_file header_files
	gcc shell.o stack.o queue.o -o main.o
main_file: shell.c
	gcc -c shell.c
header_files: ./stack/stack.h ./stack/stack.c ./queue/queue.c ./queue/queue.h
	gcc -c ./stack/stack.c
	gcc -c ./queue/queue.c
execute: main.o
	./main.o
clean:
	rm -rvf *.o
