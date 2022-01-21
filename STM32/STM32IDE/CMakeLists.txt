## Set 1: source files
set(SOURCE_FILES
  #
  # AzureRTOS
  #
  "${CMAKE_CURRENT_LIST_DIR}/AzureRTOS/App/app_azure_rtos.c"
  #
  # Core
  #
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/eth.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/gpio.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32ide_main.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/rtc.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32h7xx_hal_msp.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32h7xx_hal_timebase_tim.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32h7xx_it.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/syscalls.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/sysmem.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/tim.c"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/tx_initialize_low_level.s"
  "${CMAKE_CURRENT_LIST_DIR}/Core/Src/usart.c"
  #
  # Core/Startup
  #
  "${CMAKE_CURRENT_LIST_DIR}/Core/Startup/startup_stm32h745zitx.s"
  #
  # FileX
  #
  "${CMAKE_CURRENT_LIST_DIR}/FileX/App/app_filex.c"
  #
  # Middlewares-Interfaces: FileSystem-FileXInternalRAMInterface
  #
  "${CMAKE_CURRENT_LIST_DIR}/Middlewares/Interfaces/FileSystem/FileXInternalRAMInterface/fx_stm32_sram_driver.c"
  #
  # Common
  #
  "${CMAKE_CURRENT_LIST_DIR}/Common/system_stm32h7xx_dualcore_boot_cm4_cm7.c"
)

set_property(SOURCE
                 ${CMAKE_CURRENT_LIST_DIR}/Core/Src/tx_initialize_low_level.s 
                 ${CMAKE_CURRENT_LIST_DIR}/Core/Startup/startup_stm32h745zitx.s
             PROPERTY LANGUAGE C)

# Step 2: call `register_fprime_module` or `register_fprime_executable`
register_fprime_module()


