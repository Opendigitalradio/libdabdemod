set(LIBRARY_NAME ${${PROJECT_NAME}_LOWER})

add_library(${LIBRARY_NAME}
  demodulator.cpp
  util/fft.cpp
  util/phase_reference.cpp
  util/symbol_handler.cpp
  util/deinterleaver.cpp
  )

target_link_libraries(${LIBRARY_NAME}
  ${LIBRARY_NAME}_iface
  )

if(NOT ${${PROJECT_NAME}_UPPER}_HAS_PARENT)
  install(TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )
endif()
