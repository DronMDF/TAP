
CXX ?= g++
CXXFLAGS ?= -std=c++0x -Wall -Wextra -Weffc++ -I../../

OBJDIR ?= .obj
OBJECTS = ${patsubst %.cpp,${OBJDIR}/%.o,${wildcard *.cpp}}

all: tap-http

.PHONY: clean
clean:
	rm -f tap-http
	rm -rf ${OBJDIR}
	
tap-http: ${OBJECTS}
	${CXX} ${CXXFLAGS} -o $@ $^ -L../../ -ltap -lboost_regex
	
${OBJDIR}/%.o : %.cpp ${OBJDIR}/.keep
	${CXX} ${CXXFLAGS} -c -o $@ $<
	
${OBJDIR}/.keep:
	test -d ${OBJDIR} || mkdir ${OBJDIR}
	touch $@
