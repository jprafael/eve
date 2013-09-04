CC = g++-4.7 -std=c++11 -pthread -Wall -ffast-math -Wfatal-errors -lrt
# CC = clang++ -std=c++11 -pthread -Wall -ffast-math -Wfatal-errors -Wno-overloaded-virtual

BUILD = build
EXAMPLES = examples
PROF_GEN_FLAGS = -fprofile-generate=profile -fprofile-arcs -fprofile-values
PROF_USE_FLAGS = -fprofile-use=profile -fprofile-correction

all:
	make fib

clean:
	rm -f *~ ${BUILD}/* *.gcda

profile_%:
	${CC} -O3 ${PROF_GEN_FLAGS} ${EXAMPLES}/$*.cpp -o ${BUILD}/$*

use_%:
	${CC} -O3 ${PROF_USE_FLAGS} ${EXAMPLES}/$*.cpp -o ${BUILD}/$*

fast_%:
	make profile_$* && ./${BUILD}/$* >/dev/null 2>/dev/null && make use_$*

pg_%:
	${CC} -O2 -fno-omit-frame-pointer -pg ${EXAMPLES}/$*.cpp -o ${BUILD}/$*

debug_%:
	${CC} -g ${EXAMPLES}/$*.cpp -o ${BUILD}/$*

%:
	${CC} -O4 -DNDEBUG ${EXAMPLES}/$*.cpp -o ${BUILD}/$*
