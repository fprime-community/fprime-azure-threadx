####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/NucleoTopologyAppAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"  
  "${CMAKE_CURRENT_LIST_DIR}/Topology.cpp"  
  "${CMAKE_CURRENT_LIST_DIR}/write.c"
)
# Note: supply non-explicit dependencies here
set(MOD_DEPS
  Fw/Logger
  Svc/GroundInterface
  fprime-nucleo/NucleoTime
  fprime-nucleo/NucleoOs
  Os
)
