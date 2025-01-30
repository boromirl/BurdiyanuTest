CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -std=c++17 -g

LIB_DIRECTORY = include
LIB_SRC = ${LIB_DIRECTORY}/mylib.cc
LIB_FLAGS = -fpic -shared
LIB_TARGET = libmylib.so

LIB_NAME = mylib

all: mylib.so program1 program2

mylib.so:
	${CXX} ${LIB_FLAGS} -c ${LIB_SRC} -o ${LIB_TARGET}

# !!!! do i even need flags?
EXEC_1 = app1
EXEC_2 = app2
program1: mylib.so
	${CXX} ${CXXFLAGS} program1/app1.cc -o ${EXEC_1} -L. -l${LIB_NAME}

program2: mylib.so
	${CXX} ${CXXFLAGS} program2/main.cc -o ${EXEC_2} -L. -l${LIB_NAME}

clean:
	rm -rf test ${LIB_TARGET} *.o *.so ${EXEC_1} ${EXEC_2}

rebuild: clean all