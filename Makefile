
export CXX = g++
export CXXFLAGS = -std=c++0x -ggdb3 -O0 -Wall -Wextra -Weffc++ -I.
export LD = ld
export LDFLAGS =
export OBJDIR = .obj

OBJECTS = 
LIBS=-lrt

#tap: ${OBJECTS}
#	${CXX} -s -o $@ ${OBJECTS} ${LIBS}
	
tap_http: ${OBJDIR}/core.o ${OBJDIR}/http_client.o
	${CXX} ${CXXFLAGS} -s -o $@ tap_http.cpp \
		${OBJDIR}/core.o ${OBJDIR}/http_client.o ${LIBS}

# Тестирование
check: test
	./test --random=1

test: ${OBJDIR}/core.o ${OBJDIR}/http_client.o ${OBJDIR}/unittest.o
	${CXX} -o $@ ${OBJDIR}/core.o ${OBJDIR}/http_client.o ${OBJDIR}/unittest.o \
		${LIBS} -lboost_unit_test_framework

# Утилиты
clean:
	rm -rf ${OBJDIR}
	rm -f tap test

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
${OBJDIR}/http_client.o : ${OBJDIR}
	./build.py $@ ${basename ${notdir $@}}
