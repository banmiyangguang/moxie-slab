moxie:
	make -C src
	make -C example
	
.PHONY : clean
clean:
	make clean -C src 
	make clean -C example
