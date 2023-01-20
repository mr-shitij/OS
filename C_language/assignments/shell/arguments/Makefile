try: compile execute
compile: MainFile HeaderFiles
	gcc test_main.o arguments.o -o main.o
MainFile: test_main.c
	gcc -c test_main.c
HeaderFiles: arguments.h arguments.c
	gcc -c arguments.c
execute: main.o
	./main.o
clean:
	rm -rvf *.o
