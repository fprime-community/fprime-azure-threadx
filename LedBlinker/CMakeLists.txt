####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/LedBlinker.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/LedBlinkerComponentImpl.cpp"
)
# Check the Linux build
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nucleo-ThreadX")
    list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/LedBlinkerLinux.cpp")
# Teensyduino additions
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Nucleo-ThreadX")
    list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/LedBlinkerNucleo.cpp")
endif()
register_fprime_module()
