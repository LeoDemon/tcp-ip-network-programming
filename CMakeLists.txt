cmake_minimum_required(VERSION 3.22)
project(tcp_ip_network_programming C)

set(CMAKE_C_STANDARD 11)

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(common)
add_subdirectory(ch01)
add_subdirectory(ch02)
add_subdirectory(ch04)
add_subdirectory(ch05)
add_subdirectory(ch06)
add_subdirectory(ch07)
add_subdirectory(ch08)
add_subdirectory(ch09)
