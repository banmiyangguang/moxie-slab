cflags := -g -O3 -static -Wall -std=c++11
CC = g++

objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))
headers := $(wildcard *.h)

Main:$(objects)
	$(CC) -o $@ $^ -lslab -L../lib
%.o:%.cpp $(headers)
	$(CC) -c $<  -I./ -I../src  $(cflags) -o $@

.PHONY : clean
clean:
	rm *.o -rf 
