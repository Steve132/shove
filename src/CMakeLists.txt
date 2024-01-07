



add_library(shove INTERFACE)
target_include_directories(shove INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_compile_features(shove INTERFACE cxx_std_20)

target_sources(shove INTERFACE


${PROJECT_SOURCE_DIR}/include/shv/iterator/generator.hpp
${PROJECT_SOURCE_DIR}/include/shv/thread/true_atomic.hpp
${PROJECT_SOURCE_DIR}/include/shv/container/disjoint_set.hpp
${PROJECT_SOURCE_DIR}/include/shv/container/allocator_adaptor.hpp
${PROJECT_SOURCE_DIR}/include/shv/container/allocator_aware_container.hpp
${PROJECT_SOURCE_DIR}/include/shv/container/buffer.hpp
${PROJECT_SOURCE_DIR}/include/shv/container/polymorph.hpp
${PROJECT_SOURCE_DIR}/include/shv/meta/optional.hpp
${PROJECT_SOURCE_DIR}/include/shv/meta/min_int.hpp
)


add_library(shove_experimental INTERFACE)
target_include_directories(shove_experimental INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/incomplete>)
target_compile_features(shove_experimental INTERFACE cxx_std_20)
target_link_libraries(shove_experimental INTERFACE shove)

target_sources(shove_experimental INTERFACE

${PROJECT_SOURCE_DIR}/include/incomplete/shv/iterator/forwarding_iterator.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/threading/task_pool.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/threading/threadsafe_prng.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/threading/thread_pool.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/threading/lightweight_thread.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/simd/stdsimd_utils.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/shared_queue.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/geometric
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/geometric/list.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/geometric/src/list_stubs.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/geometric/src/list_iterator_stubs.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/geometric/src/mask.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/geometric/src/find_block.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/legacy/geometric.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/legacy/geometric_list.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/stable_iterator.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/devector.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/integer_search_tree.hpp
${PROJECT_SOURCE_DIR}/include/incomplete/shv/container/circular_deque.hpp

)
