
CXX ?= g++
CXXFLAGS ?= -O2 -g0 -Wall -Wextra -Weffc++ -Werror

OBJDIR ?= .obj
OBJECTS = ${patsubst %.cpp,${OBJDIR}/%.o,${filter-out tap-tcp-server.cpp,${wildcard *.cpp}}}

all: tap-tcp tap-tcp-server

.PHONY: clean
clean:
	rm -f tap-tcp
	rm -rf ${OBJDIR}

tap-tcp tap-tcp-server : ../../libtap.a

tap-tcp: ${OBJECTS} ../../libtap.a
	${CXX} ${CXXFLAGS} -o $@ $^ -L../.. -ltap -lboost_regex -lboost_filesystem -lboost_system 

tap-tcp-server: ${OBJDIR}/tap-tcp-server.o ../../libtap.a
	${CXX} ${CXXFLAGS} -o $@ $^ -L../.. -ltap

${OBJDIR}/%.o : %.cpp ${OBJDIR}/.keep
	${CXX} -MMD -std=c++0x ${CXXFLAGS} -I../.. -c -o $@ $<

${OBJDIR}/.keep:
	test -d ${OBJDIR} || mkdir ${OBJDIR}
	touch $@

-include ${patsubst %.o,%.d,${OBJECTS}}
-include ${OBJDIR}/tap-tcp-server.d
