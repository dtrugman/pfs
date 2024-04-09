cmake_minimum_required (VERSION 3.0)

project (find_package)

find_package (pfs)

set (SOURCES find_package.cpp)

add_executable (find_package ${SOURCES})

target_link_libraries (find_package pfs)
