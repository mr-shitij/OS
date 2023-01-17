CC=gcc
TARGET=main 

all: try run
try: DCLL.o main.o 
	$(CC) main.o  DCLL.o -o $(TARGET)
main.o: main.c DCLL.h
	$(CC) -c main.c
DCLL.o: DCLL.h DCLL.c
	$(CC) -c DCLL.c
run: $(TARGET)
	./$(TARGET)
clean:
	rm -rvf *.o $(TARGET)

