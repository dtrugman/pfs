cmake_minimum_required (VERSION 3.5)

option(BUILD_SHARED_LIBS "Build shared library (.so) instead of static one (.a)" ON)

add_compile_options (-std=c++11 -Wall -Wextra -pedantic -Werror)

include_directories (include)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set (ROOT_SOURCE_DIR src)
aux_source_directory (${ROOT_SOURCE_DIR} ROOT_SOURCES)
aux_source_directory (${ROOT_SOURCE_DIR}/parsers PARSERS_SOURCES)
set (SOURCES ${ROOT_SOURCES} ${PARSERS_SOURCES})

add_library (pfs ${SOURCES})

add_executable (sample sample/sample.cpp)
target_link_libraries (sample PRIVATE pfs)

set (UNITTEST_SOURCE_DIR test)
aux_source_directory (${UNITTEST_SOURCE_DIR} UNITTEST_SOURCES)
add_executable (unittest ${UNITTEST_SOURCES})
target_link_libraries (unittest PRIVATE pfs)
