cflags := -O3 -Wall -std=c++11 -DNDEBUG -ltcmalloc_minimal -I/opt/gperftools-2.7/include -L/opt/gperftools/lib
#cflags := -O3 -Wall -std=c++11 -ltcmalloc_minimal -lprofiler -lunwind -DNDEBUG -I/opt/gperftools-2.7/include -L/opt/gperftools/lib
CC = g++

ALL:
	$(CC) -o Slab_Hashtable Slab_Hashtable.cpp $(cflags) -lslab -L../lib -I../src
	$(CC) -o content_test content_test.cpp $(cflags) -lslab -L../lib -I../src
	$(CC) -o Unoredered_map Unoredered_map.cpp $(cflags)
	$(CC) -o MemcpyTest MemcpyTest.cpp $(cflags)

.PHONY : clean
clean:
	rm *.o -rf 
