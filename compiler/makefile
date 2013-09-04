CXX = g++-4.7
LEX = flex++
YACC = bison -v -o=output --report-file=report

GCDIR = ./runtime/eve/lib/gc

CXX_FLAGS = -std=c++11 -pthread -Wall -g
LD_FLAGS = -L/usr/local/lib/ -pthread -g

BUILD = ${PWD}/build
SRC = ${PWD}/src

GENERATED_LEX = scanner.cc
GENERATED_YACC = parser.hh parser.cc location.hh position.hh stack.hh 
GENERATED = ${GENERATED_LEX} ${GENERATED_YACC}

OBJS = ${BUILD}/eve.o ${BUILD}/driver.o ${BUILD}/parser.o ${BUILD}/scanner.o ${BUILD}/impl.o ${BUILD}/util.o

EXAMPLE = 10_fibonacci

all: eve

# main target
eve: ${SRC}/parser.cc ${SRC}/scanner.cc ${OBJS} 
	${CXX} ${OBJS} ${LD_FLAGS} -o eve

# utilities
clean:
	rm -f ${BUILD}/*
	rm -f eve
	cd ${SRC}; rm -f *~ ${GENERATED}

move_lex:
	${foreach file, ${GENERATED_LEX}, -${shell mv ${file} ${SRC}}}

move_yacc:
	${foreach file, ${GENERATED_YACC}, -${shell mv ${file} ${SRC}}}

# generate source files
${SRC}/scanner.cc: ${SRC}/scanner.ll
	${LEX} -o scanner.cc ${SRC}/scanner.ll
	make move_lex
	
${SRC}/parser.cc: ${SRC}/scanner.cc ${SRC}/parser.yy
	${YACC} -o parser.cc --defines=parser.hh ${SRC}/parser.yy
	make move_yacc

# make build directory
${BUILD}:
	mkdir ${BUILD}

# make objects
${BUILD}/%.o: ${SRC}/%.cc
	${CXX} ${CXX_FLAGS} -c ${SRC}/$*.cc -o ${BUILD}/$*.o

# compile target
example_%: all
	./eve test/$*.eve > out.cpp
	g++-4.7 -std=c++11 -pthread -Wall -ffast-math -Wfatal-errors -lrt -O3 out.cpp -o build/$*
