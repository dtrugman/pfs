cmake_minimum_required (VERSION 3.5)

project(pfs)

option(BUILD_SHARED_LIBS "Build shared library (.so) instead of static one (.a)" ON)
option(ADDRESS_SANITIZER "Enable address sanitizer" OFF)
option(BUILD_SAMPLES "Build samples" ON)
option(BUILD_TESTS "Build tests" ON)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

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

if (ADDRESS_SANITIZER MATCHES ON)
    message (STATUS "Enabling address sanitizer")
    set (SANITIZE_FLAGS -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak)
    target_compile_options (pfs PUBLIC ${SANITIZE_FLAGS})
    target_link_libraries (pfs PUBLIC -lasan ${SANITIZE_FLAGS})
endif ()

if (BUILD_SAMPLES)
    aux_source_directory (sample SAMPLE_SOURCES)
    add_executable (sample ${SAMPLE_SOURCES})
    target_link_libraries (sample PRIVATE pfs)
endif()

if (BUILD_TESTS)
    set (UNITTEST_SOURCE_DIR test)
    aux_source_directory (${UNITTEST_SOURCE_DIR} UNITTEST_SOURCES)
    add_executable (unittest ${UNITTEST_SOURCES})
    target_link_libraries (unittest PRIVATE pfs)
endif()
