
export CXX = g++
export CXXFLAGS = -std=c++0x -ggdb -O0 -Wall -Wextra -Weffc++ -I ${shell pwd}
export LD = ld
export LDFLAGS =
export OBJDIR = ${shell pwd}/.obj

OBJECTS = .obj/core.o
LIBS=-lrt

tap: ${OBJECTS}
	${CXX} -s -o $@ ${OBJECTS} ${LIBS}

# Компиляция
${OBJDIR}:
	mkdir ${OBJDIR}

${OBJDIR}/%.o : .obj
	./build.py $@ ${basename ${notdir $@}}

# Тестирование
check: test
	./test --random=1

test: ${OBJECTS} ${OBJDIR}/unittest.o
	${CXX} -o $@ ${OBJECTS} ${OBJDIR}/unittest.o ${LIBS} -lboost_unit_test_framework

# Утилиты
clean:
	rm -rf .obj
	rm -f tap test
