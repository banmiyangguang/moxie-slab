cflags := -O3 -Wall -std=c++11 -ltcmalloc_minimal -DNDEBUG -L/opt/gperftools/lib
#cflags := -O3 -Wall -std=c++11 -static -DNDEBUG
CC = g++

ALL:
	$(CC) -o Slab_Hashtable Slab_Hashtable.cpp $(cflags) -lslab -L../lib -I../src
	$(CC) -o content_test content_test.cpp $(cflags) -lslab -L../lib -I../src
	$(CC) -o Unoredered_map Unoredered_map.cpp $(cflags)
	$(CC) -o MemcpyTest MemcpyTest.cpp $(cflags)

.PHONY : clean
clean:
	rm *.o -rf 
