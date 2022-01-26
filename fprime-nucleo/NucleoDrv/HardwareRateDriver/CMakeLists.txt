####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/HardwareRateDriverComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/HardwareRateDriver.cpp"
)
# Check the Linux build
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nucleo-ThreadX")
    list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/HardwareRateDriverLinux.cpp")
# Nucleo ThreadX additions
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Nucleo-ThreadX")
    list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/HardwareRateDriverNucleo.cpp")
endif()
register_fprime_module()
