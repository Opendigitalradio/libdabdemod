find_package(PkgConfig)
pkg_check_modules(PC_FFTW3F QUIET
  fftw3f
  )

find_path(FFTW3F_INCLUDE_DIR fftw3.h
  HINTS ${PC_FFTW3F_INCLUDE_DIRS}
  )

find_library(FFTW3F_LIBRARIES NAMES
  fftw3f
  HINTS ${PC_FFTW3F_RARY_DIRS}
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW3F
  DEFAULT_MSG
  FFTW3F_LIBRARIES
  FFTW3F_INCLUDE_DIR
  )

set(FFTW3F_FOUND ${FFTW3F_FOUND})

mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_LIBRARIES)
