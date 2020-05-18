cmake_minimum_required(VERSION 2.6)

include_directories(.)

set(SOURCES
	saxy/common.hpp
	saxy/csv.hpp
	saxy/iterator.hpp
	saxy/json.hpp
	saxy/string_view.hpp
	saxy/second_throw_allocator.hpp
	saxy/stack_allocator.hpp
)

add_library(HEADER_ONLY_TARGET STATIC ${SOURCES})
set_target_properties(HEADER_ONLY_TARGET PROPERTIES LINKER_LANGUAGE CXX)
