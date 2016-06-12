set(LIB_SOURCES
  demodulator.cpp
  util/fft.cpp
  util/phase_reference.cpp
  util/symbol_handler.cpp
  util/deinterleaver.cpp
)

add_library(${PROJECT_NAME}_static STATIC ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME}_static ${CMAKE_THREAD_LIBS_INIT})

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
