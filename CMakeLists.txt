cmake_minimum_required(VERSION 3.0)
project(dabdemod CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type" FORCE)
endif()

option(BUILD_DOCUMENTATION          "Build the libdabdemod documentation." OFF)
option(BUILD_DOCUMENTATION_ONLY     "Only build the documentation not the libraries." OFF)
option(BUILD_INTERNAL_DOCUMENTATION "Generate internal developer documentation." OFF)
option(WITH_TESTS                   "Build and run the unit tests." ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/products/${CMAKE_BUILD_TYPE}" CACHE STRING "The build product prefix" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/bin" CACHE STRING "The output path for executables" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/lib" CACHE STRING "The output path for shared libraries" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" CACHE STRING "The output path for static libraries" FORCE)

set(CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Wextra -Werror")

get_directory_property(HAS_PARENT PARENT_DIRECTORY)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  option(WITH_ADDRESS_SANITIZER       "Include address/memory checks." OFF)
  option(WITH_UNDEFINED_SANITIZER     "Include undefined behavior checks." OFF)

  if(WITH_ADDRESS_SANITIZER OR WITH_UNDEFINED_SANITIZER)
    string(APPEND CMAKE_CXX_FLAGS " -fno-omit-frame-pointer")

    set(SANITIZERS "")

    if(WITH_ADDRESS_SANITIZER)
      list(APPEND SANITIZERS "address")
    endif()

    if(WITH_UNDEFINED_SANITIZER)
      list(APPEND SANITIZERS "undefined")
    endif()

    string(REPLACE ";" "," SANITIZERS "${SANITIZERS}")
    string(APPEND CMAKE_CXX_FLAGS " -fsanitize=${SANITIZERS}")
  endif()
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Os")

include_directories("include")

if(BUILD_DOCUMENTATION_ONLY)
  set(BUILD_DOCUMENTATION ON CACHE BOOL "Build the libdabdemod documentation." FORCE)
else()
  add_subdirectory(common)
  include_directories(SYSTEM ${DABCOMMON_INCLUDES})
  add_subdirectory("src")

  if(WITH_TESTS)
    include(CUTE)
    include_directories(SYSTEM "third_party/cute")
    add_subdirectory(test)
  endif()
endif()

if(BUILD_DOCUMENTATION OR BUILD_INTERNAL_DOCUMENTATION)
  set(GENERATE_INTERNAL_DOCS NO)

  if(BUILD_INTERNAL_DOCUMENTATION)
    set(GENERATE_INTERNAL_DOCS YES)

    if(NOT BUILD_DOCUMENTATION)
      set(BUILD_DOCUMENTATION ON CACHE BOOL "Build the libdabdemod documentation." FORCE)
    endif()
  endif()

  find_package(Doxygen REQUIRED)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation" VERBATIM)
endif()

if(HAS_PARENT)
  set(DABDEMOD_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
  set(DABDEMOD_LIBRARIES_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"  PARENT_SCOPE)
  set(DABDEMOD_SHARED_LIBRARIES "${PROJECT_NAME}"  PARENT_SCOPE)
  set(DABDEMOD_STATIC_LIBRARIES "${PROJECT_NAME}_static"  PARENT_SCOPE)
endif(HAS_PARENT)
