cflags := -O3 -static -Wall -std=c++11 -DNDEBUG
CC = g++

objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))
headers := $(wildcard *.h)

libslab.a:$(objects)
	ar -crv $@ $^
	mkdir -p ../lib
	mv $@ ../lib
%.o:%.cpp $(headers)
	$(CC) -c $<  -I./  $(cflags) -o $@

.PHONY : clean
clean:
	rm *.o *.a -rf 
