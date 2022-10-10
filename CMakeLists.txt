cmake_minimum_required(VERSION 3.16)

project(shove)

project(shove VERSION 0.2.0)

add_library(shove INTERFACE)
target_include_directories(shove INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(shove INTERFACE cxx_std_20)

add_library(shove_experimental INTERFACE)
target_include_directories(shove_experimental INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/incomplete>)
target_compile_features(shove INTERFACE cxx_std_20)


if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	add_subdirectory(tests)
endif()
