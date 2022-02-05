cmake_minimum_required(VERSION 3.14)

project(shove)



if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	add_subdirectory(tests)
endif()
