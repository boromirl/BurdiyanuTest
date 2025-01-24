all:
	g++ test.cc library/my_lib.cc -o test
	./test
clean:
	rm -rf test