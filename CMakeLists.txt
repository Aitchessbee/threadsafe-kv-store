cmake_minimum_required(VERSION 3.16)
project(threadsafe_kv_store LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(kv_store src/kv_store.cpp)
target_include_directories(kv_store PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(ttl_eviction src/eviction/ttl_eviction.cpp)
target_include_directories(ttl_eviction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(lru_eviction src/eviction/lru_eviction.cpp)
target_include_directories(lru_eviction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(kv_store PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(ttl_eviction PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(lru_eviction PRIVATE -Wall -Wextra -Wpedantic)

add_executable(basic_tests tests/basic_tests.cpp)
add_executable(mt_tests tests/mt_tests.cpp)

target_link_libraries(basic_tests PRIVATE kv_store ttl_eviction lru_eviction)
target_link_libraries(mt_tests PRIVATE kv_store ttl_eviction lru_eviction Threads::Threads)

target_compile_options(basic_tests PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(mt_tests PRIVATE -Wall -Wextra -Wpedantic)

add_executable(benchmark benchmarks/benchmark.cpp)
target_link_libraries(benchmark PRIVATE kv_store ttl_eviction lru_eviction Threads::Threads)
target_compile_options(benchmark PRIVATE -Wall -Wextra -Wpedantic)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(mt_tests PRIVATE -fsanitize=thread)
    target_link_options(mt_tests PRIVATE -fsanitize=thread)
endif()
