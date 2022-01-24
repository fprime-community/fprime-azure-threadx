####
# ThreadX OS CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0079 NEW)

get_directory_property(COMP_DEFS COMPILE_DEFINITIONS)

set(MOD_DEPS
        "${CMAKE_THREAD_LIBS_INIT}"
        Fw/Cfg
        Fw/Types
        Fw/Logger
        Utils/Hash
        STM32/STM32CubeH7
        STM32/STM32IDE
        )
if (${CMAKE_SYSTEM_NAME} STREQUAL "Nucleo-ThreadX")
    set(SOURCE_FILES
        # Common implementations
        "${FPRIME_FRAMEWORK_PATH}/Os/TaskCommon.cpp"
        "${FPRIME_FRAMEWORK_PATH}/Os/TaskString.cpp"
        "${FPRIME_FRAMEWORK_PATH}/Os/QueueCommon.cpp"
        "${FPRIME_FRAMEWORK_PATH}/Os/QueueString.cpp"
        # ThreadX OS virtualization implementations
        "${CMAKE_CURRENT_LIST_DIR}/Directory.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/FileSystem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/InterruptLock.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/IntervalTimer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Log.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Queue.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TaskId.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WatchdogTimer.cpp"
        # From this package: an interval timer and a stream-log timer
        "${CMAKE_CURRENT_LIST_DIR}/IntervalTimer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/StreamLog.cpp"
    )
    register_fprime_module()
endif()


####
# Dependency Melding:
#
# In short, this Os layer depends on the framework's Os layer and vise-versa. Effectively it is one
# library built in two halves, the framework base, and this add on. Here we make the link pointers
# depend on one another forming a (weak) circular dependency. This is allowed in CMake 3.13 and
# newer.
####
if (${CMAKE_SYSTEM_NAME} STREQUAL "Nucleo-ThreadX")
    add_dependencies(Os fprime-nucleo_NucleoOs)
    target_link_libraries(Os PRIVATE fprime-nucleo_NucleoOs)
    target_link_libraries(fprime-nucleo_NucleoOs PRIVATE Os)
endif()




