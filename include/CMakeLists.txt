set(LIBRARY_NAME ${${PROJECT_NAME}_LOWER}_iface)

add_library(${LIBRARY_NAME} INTERFACE)

target_include_directories(${LIBRARY_NAME} SYSTEM INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(${LIBRARY_NAME} INTERFACE
  CONAN_PKG::libdabcommon
  )

install(TARGETS ${LIBRARY_NAME}
  EXPORT ${PROJECT_NAME}
  )

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dab"
  DESTINATION "include"
  PATTERN "*.h"
  )
