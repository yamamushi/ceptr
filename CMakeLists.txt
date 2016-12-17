cmake_minimum_required(VERSION 3.5)
project(ceptr)

#-----------------------------------------------------------------------------
# Options for building

option(CEPTR_BUILD_VERBOSE       "Verbose build output." ON)

if(CEPTR_BUILD_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE true)
endif()


#-----------------------------------------------------------------------------
# Print system information

message(STATUS "CMake version:   ${CMAKE_VERSION}")
message(STATUS "System:          ${CMAKE_SYSTEM}")
message(STATUS "Processor:       ${CMAKE_SYSTEM_PROCESSOR}")

message(STATUS "Verbose:         ${CEPTR_BUILD_VERBOSE}")


#-----------------------------------------------------------------------------
# Collect into bin or lib subdirectories

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
        CACHE PATH "Single directory for all static libraries.")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
        CACHE PATH "Single directory for all dynamic libraries on Unix.")


#-----------------------------------------------------------------------------
# Check which compiler to use

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STD11)
    if(HAVE_STD11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
    else()
        message(FATAL_ERROR "No advanced standard C++ support (-std=c++11 not defined) for GCC version.")
    endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STD11)
    if(HAVE_STD11)
        if(APPLE)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -Wall -Wextra -Wno-missing-braces -Wno-unused-parameter -Wno-sign-compare -Wno-unused-private-field -Wno-mismatched-tags -U__STRICT_ANSI__")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
        endif()
    else()
        message(FATAL_ERROR "No C++11 support for Clang version. Please upgrade Clang to a version supporting C++11.")
    endif()
endif()

message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Flags:       ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ link flags:  ${CMAKE_CXX_LINK_FLAGS}")


#-----------------------------------------------------------------------------
# Force out-of-source build

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
    message(FATAL_ERROR "CMake generation is not allowed within the source directory!
Remove the CMakeCache.txt file and try again from another folder, e.g.:

   rm CMakeCache.txt
   mkdir build
   cd build
   cmake ..
")
endif()

#-----------------------------------------------------------------------------
# Default to Release build

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR}/lib)

#-----------------------------------------------------------------------------

SET(PROJECT_NAME CEPTR)
SET(CEPTR_SOURCE_FILES)

add_subdirectory(src)
add_subdirectory(spec)

add_executable(ceptr ${CEPTR_SOURCE_FILES} src/ceptr.c)