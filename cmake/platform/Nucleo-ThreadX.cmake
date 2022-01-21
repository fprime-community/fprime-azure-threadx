##
# Nucleo-ThreadX.cmake:
##
add_definitions(-DTGT_OS_TYPE_THREADX)

set(FPRIME_USE_BAREMETAL_SCHEDULE OFF)

# Add ThreadX specific headers into the system
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../fprime-nucleo/NucleoTypes")
