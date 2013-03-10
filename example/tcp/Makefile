
CXX ?= g++
CXXFLAGS ?= -O2 -g0 -std=c++0x -Wall -Wextra -Weffc++ -Werror -I../../

OBJDIR ?= .obj
OBJECTS = ${patsubst %.cpp,${OBJDIR}/%.o,${filter-out tap-tcp-server.cpp,${wildcard *.cpp}}}

all: tap-tcp tap-tcp-server

.PHONY: clean
clean:
	rm -f tap-tcp
	rm -rf ${OBJDIR}

tap-tcp tap-tcp-server : ../../libtap.a

tap-tcp: ${OBJECTS}
	${CXX} ${CXXFLAGS} -o $@ $^ -L../../ -ltap -lboost_regex -lboost_filesystem -lboost_system 

tap-tcp-server: ${OBJDIR}/tap-tcp-server.o
	${CXX} ${CXXFLAGS} -o $@ $^ -L../../ -ltap

${OBJDIR}/%.o : %.cpp ${OBJDIR}/.keep
	${CXX} -MMD ${CXXFLAGS} -c -o $@ $<

${OBJDIR}/.keep:
	test -d ${OBJDIR} || mkdir ${OBJDIR}
	touch $@

-include ${patsubst %.o,%.d,${OBJECTS}}
-include ${OBJDIR}/tap-tcp-server.d
