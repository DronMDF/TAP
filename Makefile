
CXX ?= g++
CXXFLAGS ?= -std=c++0x -Wall -Wextra -Weffc++ -I.

OBJDIR = .obj
OBJECTS = ${patsubst core/%.cpp,${OBJDIR}/%.o,${wildcard core/*.cpp}}
TEST_OBJECTS = ${patsubst unittest/%.cpp,${OBJDIR}/%.o,${wildcard unittest/*.cpp}}

all: libtap.a

check: libtap.a test
	./test --random=1

install:
	@echo TODO: implement

clean:
	rm -rf ${OBJDIR}
	rm -f libtap.a test


libtap.a: ${OBJECTS}
	${AR} -r $@ $^
	${AR} -s $@

test: ${TEST_OBJECTS}
	${CXX} -o $@ $^ -L. -ltap -lboost_unit_test_framework


${OBJDIR}/%.o : core/%.cpp ${OBJDIR}/.keep
	${CXX} ${CXXFLAGS} -c -o $@ $<

${OBJDIR}/%.o : unittest/%.cpp ${OBJDIR}/.keep
	${CXX} ${CXXFLAGS} -c -o $@ $<

${OBJDIR}/.keep:
	test -d ${OBJDIR} || mkdir ${OBJDIR}
	touch $@


# TODO: move to example
#tap-http: ${OBJDIR}/core.o ${OBJDIR}/http-client.o
#	${CXX} ${CXXFLAGS} -o $@ tap-http.cpp \
#		${OBJDIR}/core.o ${OBJDIR}/http-client.o ${LIBS}

