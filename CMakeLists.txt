cmake_minimum_required (VERSION 3.5)

project (pfs)

# ------------------------------------------------------------------------
# Flags that are expected to be shared when added as sub-directory
# ------------------------------------------------------------------------

option (pfs_BUILD_SHARED_LIBS "Build a shared library instead of static one" ${BUILD_SHARED_LIBS})
if (pfs_BUILD_SHARED_LIBS)
    set (pfs_SHARED_OR_STATIC "SHARED")
else ()
    set (pfs_SHARED_OR_STATIC "STATIC")
endif ()

if (NOT CMAKE_BUILD_TYPE)
    message (STATUS "Build type not specified. Set by default to DEBUG!")
    set (CMAKE_BUILD_TYPE Debug)
endif ()

# ------------------------------------------------------------------------
# Flags that are NOT expected to be shared when added as sub-directory
# ------------------------------------------------------------------------

option (pfs_BUILD_ASAN "Enable address sanitizer" OFF)
option (pfs_BUILD_SAMPLES "Build samples" ON)
option (pfs_BUILD_TESTS "Build tests" ON)

# ------------------------------------------------------------------------
# Actual configuration
# ------------------------------------------------------------------------

add_compile_options (-std=c++11 -Wall -Wextra -pedantic -Werror)

include_directories (include)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set (pfs_ROOT_SOURCE_DIR src)
aux_source_directory (${pfs_ROOT_SOURCE_DIR} pfs_ROOT_SOURCES)
aux_source_directory (${pfs_ROOT_SOURCE_DIR}/parsers pfs_PARSERS_SOURCES)
set (SOURCES ${pfs_ROOT_SOURCES} ${pfs_PARSERS_SOURCES})

add_library (pfs ${pfs_SHARED_OR_STATIC} ${SOURCES})

if (pfs_BUILD_ASAN)
    set (pfs_BUILD_ASAN_FLAGS -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak)
    target_compile_options (pfs PUBLIC ${pfs_BUILD_ASAN_FLAGS})
    target_link_libraries (pfs PUBLIC -lasan ${pfs_BUILD_ASAN_FLAGS})
endif ()

if (pfs_BUILD_SAMPLES)
    aux_source_directory (sample pfs_SAMPLE_SOURCES)
    add_executable (sample ${pfs_SAMPLE_SOURCES})
    target_link_libraries (sample PRIVATE pfs)
endif()

if (pfs_BUILD_TESTS)
    set (pfs_UNITTEST_SOURCE_DIR test)
    aux_source_directory (${pfs_UNITTEST_SOURCE_DIR} pfs_UNITTEST_SOURCES)
    add_executable (unittest ${pfs_UNITTEST_SOURCES})
    target_link_libraries (unittest PRIVATE pfs)
endif()

# ------------------------------------------------------------------------
# Generate a CMake package configuration file to support find_package()
# ------------------------------------------------------------------------

include(CMakePackageConfigHelpers)
include(GNUInstallDirs) # Sets a default value for CMAKE_INSTALL_LIBDIR

set (pfs_PACKAGE_CONFIG ${CMAKE_PROJECT_NAME}-config.cmake)
set (pfs_PACKAGE_CONFIG_IN ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${pfs_PACKAGE_CONFIG}.in)
set (pfs_PACKAGE_CONFIG_OUT ${CMAKE_CURRENT_BINARY_DIR}/${pfs_PACKAGE_CONFIG})

set (pfs_PACKAGE_INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

set (pfs_EXPORT_NAME ${CMAKE_PROJECT_NAME}Targets)

configure_package_config_file(
    ${pfs_PACKAGE_CONFIG_IN}
    ${pfs_PACKAGE_CONFIG_OUT}
    INSTALL_DESTINATION ${pfs_PACKAGE_INSTALL_DESTINATION}
)

install(
    TARGETS ${CMAKE_PROJECT_NAME}
    EXPORT ${pfs_EXPORT_NAME}
)

install(
    EXPORT ${pfs_EXPORT_NAME}
    FILE ${pfs_PACKAGE_CONFIG}
    DESTINATION ${pfs_PACKAGE_INSTALL_DESTINATION}
)

install(
    DIRECTORY include/
    DESTINATION include
)
