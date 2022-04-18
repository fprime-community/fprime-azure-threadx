####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
if(${CMAKE_SYSTEM_NAME} STREQUAL "Nucleo-ThreadX")
  set(SOURCE_FILES
      "${FPRIME_FRAMEWORK_PATH}/Svc/Time/Time.fpp"
      "${CMAKE_CURRENT_LIST_DIR}/NucleoTimeImpl.cpp"
      )
else()
  set(SOURCE_FILES
      "${FPRIME_FRAMEWORK_PATH}/Svc/Time/Time.fpp"
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimeImpl.cpp"
      )
endif()
set(MOD_DEPS
  Svc/Time
)
register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/Time/Time.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Main.cpp"
)
register_fprime_ut()
