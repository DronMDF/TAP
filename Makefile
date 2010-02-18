
objname=${addprefix .obj/, ${addsuffix .o, ${basename ${notdir ${1}}}}}
depname=${addprefix .dep/, ${addsuffix .d, ${basename ${notdir ${1}}}}}

SOURCES=${wildcard ./*.cpp}
OBJECTS=${call objname, ${SOURCES}}

UTSOURCES=${wildcard ./unittest/*.cpp}
UTOBJECTS=${call objname, ${UTSOURCES}}

CXX=g++
CXXFLAGS=-ggdb -O0 -Wall -Wextra -Weffc++
LIBS=-lrt

.PHONY: begin depend clean test check

all: test xap

xap: ${OBJECTS}
	${CXX} -s -o $@ ${OBJECTS} ${LIBS}

# Компиляция
.obj/%.o: %.cpp .dep/%.d
	@if [ ! -d .obj ]; then mkdir .obj; fi
	${CXX} ${CXXFLAGS} -c -o $@ $<

.obj/%.o: unittest/%.cpp .dep/%.d
	@if [ ! -d .obj ]; then mkdir .obj; fi
	${CXX} ${CXXFLAGS} -c -o $@ $<

# Тестирование
test: .test/runner
	.test/runner --random=1

.test/runner: ${filter-out .obj/xap.o, ${OBJECTS}} ${UTOBJECTS}
	@if [ ! -d .test ]; then mkdir .test; fi
	${CXX} -o $@ ${filter-out .obj/xap.o, ${OBJECTS}} ${UTOBJECTS} \
		${LIBS} -lboost_unit_test_framework

# Генерация зависимостей
depend: ${call depname, ${SOURCES} ${UTSOURCES}}

.dep/%.d: %.cpp ${wildcards *.h}
	@if [ ! -d .dep ]; then mkdir .dep; fi
	${CXX} ${CXXFLAGS} -MM -MT ${call objname, $<}  -MT ${call depname, $<} $< >$@

.dep/%.d: unittest/%.cpp ${wildcards *.h unittest/*.h}
	@if [ ! -d .dep ]; then mkdir .dep; fi
	${CXX} ${CXXFLAGS} -MM -MT ${call objname, $<} -MT ${call depname, $<} $< >$@

-include ${call depname, ${SOURCES} ${UTSOURCES}}

# Утилиты
clean:
	rm -rf .dep .obj .test
	rm -f xap
