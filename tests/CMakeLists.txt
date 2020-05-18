cmake_minimum_required(VERSION 2.6)

set(Catch_INCLUDE_DIR NOTFOUND CACHE PATH "Path to Catch include directory")

include_directories(${Catch_INCLUDE_DIR})
include_directories(../include/)

set(SOURCES
	test/common_test.cpp
	test/string_view_test.cpp
	test/csv_test.cpp
	test/main.cpp

	test/util/tools.hpp
)

add_executable(unit_tests ${SOURCES})
