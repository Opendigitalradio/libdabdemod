set(LIB_SOURCES
  demodulator.cpp
  util/fft.cpp
)

add_library(${PROJECT_NAME}_static STATIC ${LIB_SOURCES})
add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
