##
# Nucleo-ThreadX.cmake:
##
add_definitions(-DTGT_OS_TYPE_THREADX)
add_definitions(-DNUCLEO)

set(FPRIME_USE_BAREMETAL_SCHEDULE OFF)

# Add ThreadX specific headers into the system
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../fprime-nucleo/NucleoTypes")

# Add Azure headers into the system
include_directories(${AZURE_PATH}/libs/threadx
                    ${AZURE_PATH}/libs/filex
                    ${AZURE_PATH}/core/filex/common/inc
                    ${AZURE_PATH}/core/filex/ports/cortex_m7/gnu/inc
                    ${AZURE_PATH}/core/threadx/ports/cortex_m7/gnu/inc
                    ${AZURE_PATH}/core/threadx/common/inc
                    )
