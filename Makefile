
export CXX = g++
export CXXFLAGS = -std=c++0x -ggdb3 -O0 -Wall -Wextra -Weffc++ -I.
export LD = ld
export LDFLAGS =
export OBJDIR = .obj

OBJECTS = 
LIBS=

#tap: ${OBJECTS}
#	${CXX} -s -o $@ ${OBJECTS} ${LIBS}
	
tap-http: ${OBJDIR}/core.o ${OBJDIR}/http-client.o
	${CXX} ${CXXFLAGS} -o $@ tap-http.cpp \
		${OBJDIR}/core.o ${OBJDIR}/http-client.o ${LIBS}

# Тестирование
check: test
	./test --random=1

test: ${OBJDIR}/core.o ${OBJDIR}/http-client.o ${OBJDIR}/unittest.o
	${CXX} -o $@ ${OBJDIR}/core.o ${OBJDIR}/http-client.o ${OBJDIR}/unittest.o \
		${LIBS} -lboost_unit_test_framework

# Утилиты
clean:
	rm -rf ${OBJDIR}
	rm -f tap-http test

# Компиляция
${OBJDIR}:
	mkdir ${OBJDIR}

.PHONY: ${OBJDIR}/core.o
${OBJDIR}/core.o : ${OBJDIR}
	./build.py $@ ${basename ${notdir $@}}

.PHONY: ${OBJDIR}/unittest.o
${OBJDIR}/unittest.o : ${OBJDIR}
	./build.py $@ ${basename ${notdir $@}}

.PHONY: ${OBJDIR}/http_client.o
${OBJDIR}/http-client.o : ${OBJDIR}
	./build.py $@ ${basename ${notdir $@}}

