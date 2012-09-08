
CXX ?= g++
CXXFLAGS ?= -std=c++0x -Wall -Wextra -Weffc++ -I../../

OBJDIR ?= .obj
OBJECTS = ${patsubst %.cpp,${OBJDIR}/%.o,${wildcard *.cpp}}

all: tap-tcp

.PHONY: clean
clean:
	rm -f tap-tcp
	rm -rf ${OBJDIR}

tap-tcp: ../../libtap.a

tap-tcp: ${OBJECTS}
	${CXX} ${CXXFLAGS} -o $@ $^ -L../../ -ltap -lboost_regex

${OBJDIR}/%.o : %.cpp ${OBJDIR}/.keep
	${CXX} -MMD ${CXXFLAGS} -c -o $@ $<

${OBJDIR}/.keep:
	test -d ${OBJDIR} || mkdir ${OBJDIR}
	touch $@
