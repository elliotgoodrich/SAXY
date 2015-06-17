cmake_minimum_required (VERSION 2.6)
project(SAXY)
set(SAXY_VERSION_MAJOR 0)
set(SAXY_VERSION_MINOR 1)

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror -std=c++1y -O3 -fno-omit-frame-pointer")
endif()

if(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

add_subdirectory(include)
add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(benchmarks)
enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)

