# Disable targets not supported on Nucleo
set_target_properties(
    Svc_LinuxTime
    Drv_SocketIpDriver
    PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/fprime-nucleo/NucleoOs")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/fprime-nucleo/NucleoTime")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/fprime-nucleo/NucleoDrv/SerialDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/fprime-nucleo/NucleoDrv/HardwareRateDriver")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/STM32")
