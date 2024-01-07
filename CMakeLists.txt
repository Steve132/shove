cmake_minimum_required(VERSION 3.16)

project(shove VERSION 0.2.1)

add_subdirectory(src)


if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	add_subdirectory(tests)
endif()
