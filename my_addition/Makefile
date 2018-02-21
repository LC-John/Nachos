GCC = gcc

all: helloworld

helloworld: main.o sayhi.o count.o
	$(GCC) -o helloworld main.o sayhi.o count.o

main.o: main.c sayhi.h count.h
	$(GCC) -c main.c

sayhi.o: sayhi.c sayhi.h
	$(GCC) -c sayhi.c

count.o: count.c count.h
	$(GCC) -c count.c

clean:
	rm helloworld
	rm *.o