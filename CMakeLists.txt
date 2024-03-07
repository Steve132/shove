cmake_minimum_required(VERSION 3.16)

project(shove VERSION 0.2.1)


include(FetchContent)
FetchContent_Declare(
	  cmake_helpers
  GIT_REPOSITORY "git@github.com:Steve132/cmake_helpers.git"
  GIT_SHALLOW TRUE
  GIT_TAG "master"
)
FetchContent_MakeAvailable(cmake_helpers)
include("${cmake_helpers_SOURCE_DIR}/GenerateDisassembly.cmake")

add_subdirectory(src)


if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	add_subdirectory(tests)
endif()
