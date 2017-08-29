cmake_minimum_required(VERSION 3.1)
project("DABDemod"
  LANGUAGES CXX
  VERSION 1.0.2)

string(TOUPPER ${PROJECT_NAME} ${PROJECT_NAME}_UPPER)
string(TOLOWER ${PROJECT_NAME} ${PROJECT_NAME}_LOWER)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config/Base.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config/Options.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config/Dependencies.cmake")

include_directories("include")

add_subdirectory("include")
add_subdirectory("src")

if(${${PROJECT_NAME}_UPPER}_ENABLE_TESTS)
  include("CUTE")
  add_subdirectory(test)
endif()
