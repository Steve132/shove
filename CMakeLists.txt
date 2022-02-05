cmake_minimum_required(VERSION 3.14)

project(shove)

project(shove VERSION 0.2.0)

add_library(shove INTERFACE)
target_include_directories(shove INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(shove INTERFACE cxx_std_17)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	add_subdirectory(tests)
endif()
