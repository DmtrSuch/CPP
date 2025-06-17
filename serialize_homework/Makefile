CC=g++
FLAGS=-std=c++20 -Wall -pedantic -Wextra
TEST_LIBRARY=-lgtest -lgtest_main -lpthread

all: build

build:
	$(CC) $(FLAGS) test.cpp -o test $(TEST_LIBRARY)

tests:
	./test

clean:
	rm -f *.o test

