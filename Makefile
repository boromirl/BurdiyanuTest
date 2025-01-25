CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -std=c++17 -g

LIB_DIRECTORY = include
LIB_SRC = ${LIB_DIRECTORY}/my_lib.cc
LIB_FLAGS = -fpic -shared
LIB_TARGET = mylib.so

mylib.so:
	${CXX} ${LIB_FLAGS} ${LIB_SRC} -I${LIB_DIRECTORY} -o ${LIB_TARGET}

test:
	${CXX} ${CXXFLAGS} test.cc -o test -L. -lmylib -Wl,-rpath=.

clean:
	rm -rf test ${LIB_TARGET} *.o *.so

# g++ -fPIC -c include/my_lib.cc
# g++ -shared -o libmy_lib.so my_lib.o
# g++ -o test test.cc -L. -lmylib