####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/NucleoTopologyDefs.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/write.c"
)
# Note: supply non-explicit dependencies here
set(MOD_DEPS
  Fw/Logger
  Svc/GroundInterface
  fprime-nucleo/NucleoDrv/SerialDriver
  fprime-nucleo/NucleoDrv/HardwareRateDriver
  fprime-nucleo/NucleoTime
  fprime-nucleo/NucleoOs
  Os
)

register_fprime_module()