cmake_minimum_required(VERSION 3.1)
project("DABDemod"
  LANGUAGES CXX
  VERSION 1.0.1)

string(TOUPPER ${PROJECT_NAME} ${PROJECT_NAME}_UPPER)
string(TOLOWER ${PROJECT_NAME} ${PROJECT_NAME}_LOWER)

option(${${PROJECT_NAME}_UPPER}_ENABLE_TESTS
  "Build and run the ${PROJECT_NAME} unit tests."
  OFF
  )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
set(CXX_VERSION "c++11")

include("CompilerSetup")
include("ConanPackages")

install_conan_packages(SYSTEM_HEADERS
  PKGOPTS ${CONAN_OPTIONS}
  )

include_directories("include")

add_subdirectory("include")
add_subdirectory("src")

if(${${PROJECT_NAME}_UPPER}_ENABLE_TESTS)
  include("CUTE")
  add_subdirectory(test)
endif()

install(EXPORT ${PROJECT_NAME}
  DESTINATION share/${${PROJECT_NAME}_LOWER}/cmake
  )

export(EXPORT ${PROJECT_NAME})
