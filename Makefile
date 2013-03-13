
CXX ?= g++
CXXFLAGS ?= -O2 -g0 -Wall -Wextra -Weffc++ -Werror

OBJDIR = .obj
OBJECTS = ${patsubst core/%.cpp,${OBJDIR}/%.o,${wildcard core/*.cpp}}
TEST_OBJECTS = ${patsubst unittest/%.cpp,${OBJDIR}/%.o,${wildcard unittest/*.cpp}}
.PHONY: check install examples

all: libtap.a

check: libtap.a test
	./test --random=1

install:
	@echo TODO: implement

clean:
	rm -rf ${OBJDIR}
	rm -f libtap.a test
	make -C example/tcp $@
	make -C example/http $@

libtap.a: ${OBJECTS}
	${AR} -r $@ $^
	${AR} -s $@

test: ${TEST_OBJECTS} libtap.a
	${CXX} -o $@ $^ -L. -ltap -lboost_unit_test_framework

examples: libtap.a
	${MAKE} ${MAKEFLAGS} -C example/tcp
	${MAKE} ${MAKEFLAGS} -C example/http

${OBJDIR}/%.o : core/%.cpp ${OBJDIR}/.keep
	${CXX} -MMD -std=c++0x ${CXXFLAGS} -I. -c -o $@ $<

${OBJDIR}/%.o : unittest/%.cpp ${OBJDIR}/.keep
	${CXX} -MMD -std=c++0x ${CXXFLAGS} -I. -c -o $@ $<

${OBJDIR}/.keep:
	test -d ${OBJDIR} || mkdir ${OBJDIR}
	touch $@

-include ${patsubst %.o,%.d,${OBJECTS}}
-include ${patsubst %.o,%.d,${TEST_OBJECTS}}
