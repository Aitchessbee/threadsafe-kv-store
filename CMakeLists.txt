cmake_minimum_required(VERSION 3.16)
project(threadsafe_kv_store LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(snapshot_manager src/persistence/snapshot_manager.cpp)
target_include_directories(snapshot_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(ttl_eviction src/eviction/ttl_eviction.cpp)
target_include_directories(ttl_eviction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(lru_eviction src/eviction/lru_eviction.cpp)
target_include_directories(lru_eviction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(kv_store src/kv_store.cpp)
target_include_directories(kv_store PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(kv_store PUBLIC snapshot_manager ttl_eviction lru_eviction Threads::Threads)

set(COMMON_OPTS -Wall -Wextra -Wpedantic)
target_compile_options(snapshot_manager PRIVATE ${COMMON_OPTS})
target_compile_options(ttl_eviction PRIVATE ${COMMON_OPTS})
target_compile_options(lru_eviction PRIVATE ${COMMON_OPTS})
target_compile_options(kv_store PRIVATE ${COMMON_OPTS})

add_executable(kv_store_cli src/main.cpp)
target_link_libraries(kv_store_cli PRIVATE kv_store)
target_compile_options(kv_store_cli PRIVATE ${COMMON_OPTS})

add_executable(basic_tests tests/basic_tests.cpp)
target_link_libraries(basic_tests PRIVATE kv_store)
target_compile_options(basic_tests PRIVATE ${COMMON_OPTS})

add_executable(mt_tests tests/mt_tests.cpp)
target_link_libraries(mt_tests PRIVATE kv_store)
target_compile_options(mt_tests PRIVATE ${COMMON_OPTS})

add_executable(benchmark benchmarks/benchmark.cpp)
target_link_libraries(benchmark PRIVATE kv_store)
target_compile_options(benchmark PRIVATE ${COMMON_OPTS})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(mt_tests PRIVATE -fsanitize=thread)
    target_link_options(mt_tests PRIVATE -fsanitize=thread)
endif()
