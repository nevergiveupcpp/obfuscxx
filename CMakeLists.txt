cmake_minimum_required(VERSION 3.15)
project(obfuscxx VERSION 1.3.1 LANGUAGES CXX)

add_library(obfuscxx INTERFACE)
add_library(obfuscxx::obfuscxx ALIAS obfuscxx)

target_include_directories(
        obfuscxx INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
)

install(TARGETS obfuscxx
        EXPORT obfuscxxTargets
)

install(EXPORT obfuscxxTargets
        FILE obfuscxxTargets.cmake
        NAMESPACE obfuscxx::
        DESTINATION lib/cmake/obfuscxx
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/obfuscxxConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/obfuscxxConfig.cmake
        INSTALL_DESTINATION lib/cmake/obfuscxx
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/obfuscxxConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/obfuscxxConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/obfuscxxConfigVersion.cmake
        DESTINATION lib/cmake/obfuscxx
)