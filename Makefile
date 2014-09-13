all: main

CC=g++

CFLAGS= -Wall -std=c++11

main: main.cpp V3.cpp M33.cpp
	$(CC) $(CFLAGS) -o main main.cpp V3.cpp M33.cpp

run:
	./main

clean:
	rm main *.csv
