
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(CMAKE_CXX_STANDARD 17)
add_executable(polymorph_test polymorph_test.cpp)

set(CMAKE_CXX_STANDARD 17)
add_executable(allocator_aware_container_test allocator_aware_container_test.cpp)

set(CMAKE_CXX_STANDARD 20)
add_executable(geometric_list_test geometric_list_test.cpp)

set(CMAKE_CXX_STANDARD 20)
add_executable(geometric_list_benchmar geometric_list_benchmark.cpp)
