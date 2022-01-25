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
                    
# Add STM32H7 drivers headers into the system
include_directories(${STM32H7_FILES_PATH}/Drivers/BSP/STM32H7xx_Nucleo
                    ${STM32H7_FILES_PATH}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
                    ${STM32H7_FILES_PATH}/Drivers/CMSIS/Include
                    ${STM32H7_FILES_PATH}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
                    ${STM32H7_FILES_PATH}/Drivers/STM32H7xx_HAL_Driver/Inc)
                    
# Add STM32IDE headers into the system
include_directories(${STM32IDE_FILES_PATH}/CM7/AzureRTOS/App
                    ${STM32IDE_FILES_PATH}/CM7/Core/Inc
                    ${STM32IDE_FILES_PATH}/CM7/Core/Inc
                    ${STM32IDE_FILES_PATH}/CM7/FileX/App
                    ${STM32IDE_FILES_PATH}/CM7/FileX/Target)
