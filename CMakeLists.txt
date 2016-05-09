cmake_minimum_required(VERSION 3.0)
project(dabdecode CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type" FORCE)
endif()

option(BUILD_DOCUMENTATION          "Build the libdabip documentation." OFF)
option(BUILD_DOCUMENTATION_ONLY     "Only build the documentation not the libraries." OFF)
option(BUILD_INTERNAL_DOCUMENTATION "Generate internal developer documentation." OFF)
option(WITH_TESTS                   "Build and run the unit tests." ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/products/${CMAKE_BUILD_TYPE}" CACHE STRING "The build product prefix")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/${OUTPUT_PREFIX}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "/${OUTPUT_PREFIX}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "/${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

set(CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Os")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined")
endif()

include_directories("include")
include_directories(SYSTEM "third_party/cute")

if(BUILD_DOCUMENTATION_ONLY)
  set(BUILD_DOCUMENTATION          ON CACHE BOOL "Build the libdabip documentation." FORCE)
else()
  add_subdirectory("src")

  if(WITH_TESTS)
    include(CUTE)
    add_subdirectory(test)
  endif()
endif()

if(BUILD_DOCUMENTATION OR BUILD_INTERNAL_DOCUMENTATION)
  set(GENERATE_INTERNAL_DOCS NO)

  if(BUILD_INTERNAL_DOCUMENTATION)
    set(GENERATE_INTERNAL_DOCS YES)

    if(NOT BUILD_DOCUMENTATION)
      set(BUILD_DOCUMENTATION          ON CACHE BOOL "Build the libdabip documentation." FORCE)
    endif()
  endif()

  find_package(Doxygen REQUIRED)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation" VERBATIM)
endif()

