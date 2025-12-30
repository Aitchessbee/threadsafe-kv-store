cmake_minimum_required(VERSION 3.16)
project(threadsafe_kv_store LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library target
add_library(kv_store
    src/kv_store.cpp
)

target_include_directories(kv_store
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Test executable
add_executable(basic_tests
    tests/basic_tests.cpp
)

target_link_libraries(basic_tests
    PRIVATE kv_store
)

# Enable warnings (recommended)
target_compile_options(kv_store PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(basic_tests PRIVATE -Wall -Wextra -Wpedantic)
