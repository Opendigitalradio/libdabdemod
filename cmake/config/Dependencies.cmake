include("ExternalDependency")

set(${${PROJECT_NAME}_UPPER}_DEPS)

if(EXTERNAL_DEPS_VIA STREQUAL "conan")
  include("ConanPackages")
  install_conan_packages(SYSTEM_HEADERS
    PKGOPTS ${CONAN_OPTIONS}
    )
  list(APPEND ${${PROJECT_NAME}_UPPER}_DEPS
    CONAN_PKG::libdabcommon
    )
elseif(EXTERNAL_DEPS_VIA STREQUAL "git")
  external_dependency(CMAKE
    NAME    "dabcommon"
    REPO    "https://github.com/Opendigitalradio/libdabcommon"
    )
else()
  message(FATAL_ERROR "Unknown dependency resolution mechanism '${EXTERNAL_DEPS_VIA}'")
endif()

find_package(FFTW3F REQUIRED)

list(APPEND ${${PROJECT_NAME}_UPPER}_DEPS
  ${FFTW3F_LIBRARIES}
  )
