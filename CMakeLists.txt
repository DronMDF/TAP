cmake_minimum_required(VERSION 2.6)

set(PROJECT xap)

project(${PROJECT})
enable_testing()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -ggdb")

# Главный бинарь
file(GLOB SOURCES *.cpp *.h)
add_executable(${PROJECT} ${SOURCES})

# Тест раннер
set(TEST test_${PROJECT})
file(GLOB TEST_SOURCES unittest/*.cpp)
list(APPEND TEST_SOURCES ${SOURCES})
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/xap.cpp)
add_executable(${TEST} ${TEST_SOURCES})
target_link_libraries(${TEST} ${Boost_LIBRARIES})
add_test(${TEST} ${TEST})

# Может это и не очень правильно я написал, разберемся...
add_custom_target(distclean
	COMMAND make clean
	COMMAND cat ".hgignore" | xargs rm -rf
	COMMENT Cleaning all generated files...)
