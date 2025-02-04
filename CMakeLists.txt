# root
cmake_minimum_required(VERSION 3.22)
project("WildSentinel Common" VERSION 0.1.0 LANGUAGES C CXX)

# Set CMake policies
cmake_policy(VERSION ${CMAKE_VERSION})
if(POLICY CMP0021)
    cmake_policy(SET CMP0021 NEW)
endif()

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

if(POLICY CMP0156)
    cmake_policy(SET CMP0156 NEW)
endif()

if(POLICY CMP0179)
    cmake_policy(SET CMP0179 NEW)
endif()

option(WSCOMMON_BUILD_IMAGING "Build imaging and pixel modules" ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release)" FORCE)
endif()

# Set installation prefix
if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "Default installation prefix" FORCE)
endif()

# Confirm the installation prefix
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# Global position independent code setting
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configure flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Using Debug configuration")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Using Release configuration")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -flto -march=native")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--strip-all")
endif()

# Global settings
set(BUILD_THIRDPARTY ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/)
set(WSCOMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(WSCOMMON_LIB_DIR ${WSCOMMON_SOURCE_DIR}/lib/)
set(WSCOMMON_BIN_DIR ${WSCOMMON_SOURCE_DIR}/bin/)

message(STATUS "PROJECT_NAME ${PROJECT_NAME}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Set RPATH
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_RPATH "$ORIGIN/../lib")

# Build the library (src)
add_subdirectory(src)
