cmake_minimum_required(VERSION 3.15)
project(unittest-obfuscxx VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(unittest-obfuscxx
        unittest.cpp
)

target_include_directories(unittest-obfuscxx PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(unittest-obfuscxx PRIVATE
        Threads::Threads
        GTest::gtest
        GTest::gtest_main
)

target_compile_options(unittest-obfuscxx PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/constexpr:steps10000000>
        $<$<CXX_COMPILER_ID:GNU>:-mavx2>
        $<$<CXX_COMPILER_ID:Clang>:-mavx2>
)

target_compile_definitions(unittest-obfuscxx PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:OBFUSCXX_MSVC_FORCE_AVX2>
)

enable_testing()
add_test(NAME unittest-obfuscxx COMMAND unittest-obfuscxx)