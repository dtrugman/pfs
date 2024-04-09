cmake_minimum_required (VERSION 3.5)

project (find_package)

find_package (pfs PATHS /pfs NO_DEFUALT_PATH REQUIRED)

set (SOURCES find_package.cpp)

add_executable (find_package ${SOURCES})

target_link_libraries (find_package -L${pfs_LIBRARY_DIR} ${pfs_LIBRARIES})
target_include_directories (find_package PRIVATE ${pfs_INCLUDE_DIRS})
