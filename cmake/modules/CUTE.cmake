enable_testing()

set(ENV{CTEST_OUTPUT_ON_FAILURE} "1")
set(CUTE_TEST_REPORTS_DIRECTORY ${CMAKE_BINARY_DIR}/cute-reports)

if(NOT WIN32)
  string(ASCII 27 ESC)
  set(RST "${ESC}[m")
  set(BLD "${ESC}[1m")
  set(RED "${ESC}[31m")
  set(GRN "${ESC}[32m")
  set(YEL "${ESC}[33m")
  set(BLU "${ESC}[34m")
  set(MAG "${ESC}[35m")
  set(CYA "${ESC}[36m")
  set(WHI "${ESC}[37m")
  set(BRE "${ESC}[1;31m")
  set(BGR "${ESC}[1;32m")
  set(BYE "${ESC}[1;33m")
  set(BBL "${ESC}[1;34m")
  set(BMA "${ESC}[1;35m")
  set(BCY "${ESC}[1;36m")
  set(BWH "${ESC}[1;37m")
endif()

file(MAKE_DIRECTORY ${CUTE_TEST_REPORTS_DIRECTORY})

function(cute_test TEST_NAME)
  set(TEST_SOURCE_FILE "${TEST_NAME}_test.cpp")
  set(TEST_TARGET_NAME "${TEST_NAME}_test")

  file(STRINGS "${TEST_SOURCE_FILE}" LIBRARIES REGEX "//[ \t]*@CMAKE_CUTE_LIBRARY=.*")
  file(STRINGS "${TEST_SOURCE_FILE}" DEPENDENCIES REGEX "//[ \t]*@CMAKE_CUTE_DEPENDENCY=.*")

  set(LIBS "")
  set(DEPS "")

  foreach(LIBRARY IN LISTS LIBRARIES)
    string(REGEX REPLACE "//[ \t]*@CMAKE_CUTE_LIBRARY=(.*)" "\\1" LIBRARY ${LIBRARY})
    list(APPEND LIBS ${LIBRARY})
  endforeach()

  foreach(DEPENDENCY IN LISTS DEPENDENCIES)
    string(REGEX REPLACE "//[ \t]*@CMAKE_CUTE_DEPENDENCY=(.*)" "\\1" DEPENDENCY ${DEPENDENCY})
    list(APPEND DEPS "${CMAKE_SOURCE_DIR}/${DEPENDENCY}")
  endforeach()

  message(STATUS "${BYE}Adding test ${BBL}${TEST_NAME}${RST}")

  add_executable(${TEST_TARGET_NAME} ${TEST_SOURCE_FILE} ${DEPS})

  if(TARGET ${TEST_TARGET_NAME})
    target_link_libraries(${TEST_TARGET_NAME} ${LIBS})
    add_test(NAME ${TEST_TARGET_NAME} WORKING_DIRECTORY ${CUTE_TEST_REPORTS_DIRECTORY} COMMAND ${TEST_TARGET_NAME})
    if(NOT CUTE_SKIP_RUN_UNIT_TESTS)
      add_custom_target(cute_${TEST_TARGET_NAME} ALL
                        COMMAND ctest -R ${TEST_TARGET_NAME} -Q --output-on-failure
                        DEPENDS ${TEST_TARGET_NAME}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMENT "Running CUTE test ${TEST_NAME} ..."
                        VERBATIM)
    endif(NOT CUTE_SKIP_RUN_UNIT_TESTS)
  endif()
endfunction()
