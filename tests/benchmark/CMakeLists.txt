cmake_minimum_required(VERSION 3.15)
project(benchmark-obfuscxx VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(benchmark CONFIG REQUIRED)

add_executable(benchmark-obfuscxx
        benchmark.cpp
)

target_include_directories(benchmark-obfuscxx PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(benchmark-obfuscxx PRIVATE
        Threads::Threads
        benchmark::benchmark
        benchmark::benchmark_main
)

target_compile_options(benchmark-obfuscxx PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/constexpr:steps10000000>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},aarch64>>>:-mavx2>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},aarch64>>>:-mavx2>
)

target_compile_definitions(benchmark-obfuscxx PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:OBFUSCXX_MSVC_FORCE_AVX2>
)

enable_testing()
add_test(NAME benchmark-obfuscxx COMMAND benchmark-obfuscxx)