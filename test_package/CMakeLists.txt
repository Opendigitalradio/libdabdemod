cmake_minimum_required(VERSION 3.1.2)
project(PackageTest
  LANGUAGES CXX
  VERSION 1.0.1
  )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
set(CONAN_SYSTEM_INCLUDES ON)
conan_basic_setup(TARGETS)

add_executable(package_test package_test.cpp)
target_link_libraries(package_test CONAN_PKG::libdabdemod)
