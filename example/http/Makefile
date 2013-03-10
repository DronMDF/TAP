
CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Weffc++ -Werror

OBJDIR ?= .obj
OBJECTS = ${patsubst %.cpp,${OBJDIR}/%.o,${wildcard *.cpp}}

all: tap-http

.PHONY: clean
clean:
	rm -f tap-http
	rm -rf ${OBJDIR}

tap-http: ../../libtap.a

tap-http: ${OBJECTS}
	${CXX} ${CXXFLAGS} -o $@ $^ -L../.. -ltap -lboost_regex
	
${OBJDIR}/%.o : %.cpp ${OBJDIR}/.keep
	${CXX} -MMD -std=c++0x ${CXXFLAGS}  -I../.. -c -o $@ $<
	
${OBJDIR}/.keep:
	test -d ${OBJDIR} || mkdir ${OBJDIR}
	touch $@

-include ${patsubst %.o,%.d,${OBJECTS}}
	