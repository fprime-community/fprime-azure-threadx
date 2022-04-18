# Set system name
set(CMAKE_SYSTEM_NAME Nucleo-ThreadX)
set(CMAKE_SYSTEM_VERSION 0.1)
set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
set(CMAKE_CROSSCOMPILING 1)

# Set the build environment variables
set(DEVELOP_REPOS_BASE "/home/cmjl/projects/FPrime")
set(STM32_TOOLS_PATH "/opt/st/stm32cubeide_1.7.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.linux64_2.0.0.202105311346/tools")
set(STM32CUBEH7_REPO_PATH "${DEVELOP_REPOS_BASE}/STM32CubeH7")
set(STM32H7_FILES_PATH "${DEVELOP_REPOS_BASE}/fprime-azure-threadx/STM32/STM32CubeH7")
set(STM32IDE_FILES_PATH "${DEVELOP_REPOS_BASE}/fprime-azure-threadx/STM32/STM32IDE")
set(STM32_LD_SCRIPTS_PATH "${STM32IDE_FILES_PATH}/LinkerScripts")
set(AZURE_PATH "${DEVELOP_REPOS_BASE}/azure-libs")
set(AZURE_LIBS_PATH "${AZURE_PATH}/build/libs")
set(STM_COMPILER_ROOT "${STM32_TOOLS_PATH}")
set(LINKER_SCRIPT "${STM32_LD_SCRIPTS_PATH}/CM7/STM32H745ZITX_FLASH.ld")

# Check toolchain directory exists
IF(NOT EXISTS "${STM_COMPILER_ROOT}")
    message(FATAL_ERROR " STM compilers not found at ${STM_COMPILER_ROOT}.")
endif()
message(STATUS "Using STM compilers at: ${STM_COMPILER_ROOT}")

# specify the cross compiler
set(CMAKE_C_COMPILER "${STM_COMPILER_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${STM_COMPILER_ROOT}/bin/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${STM_COMPILER_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_AR "${STM_COMPILER_ROOT}/bin/arm-none-eabi-ar")
set(CMAKE_RANLIB "${STM_COMPILER_ROOT}/bin/arm-none-eabi-ranlib")
set(CMAKE_C_ARCHIVE_CREATE "${CMAKE_AR} cr <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "${CMAKE_AR} r  <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH "${CMAKE_RANLIB} <TARGET>")
set(CMAKE_CXX_ARCHIVE_CREATE "${CMAKE_C_ARCHIVE_CREATE}")
set(CMAKE_CXX_ARCHIVE_APPEND "${CMAKE_C_ARCHIVE_APPEND}")
set(CMAKE_CXX_ARCHIVE_FINISH "${CMAKE_C_ARCHIVE_FINISH}")

#set(CMAKE_CXX_LINK_EXECUTABLE "${STM32_LD_SCRIPTS_PATH}/CM7/STM32H745ZITX_FLASH.ld -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY" CACHE STRING "Try Static Lib Type" FORCE)

# MCU and VFP flags
set(MCPU_FLAGS "-mthumb -mcpu=cortex-m7")
set(VFP_FLAGS "-mfpu=fpv5-d16 -mfloat-abi=hard")

# Toolchain preprocessor definitions
set(COMPILER_P_PROC_DEFS "-DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H745xx -DUSE_FULL_LL_DRIVER")
# Application preprocessor definitions
set(APP_PREC_DEFS "-DTX_INCLUDE_USER_DEFINE_FILE -DFX_INCLUDE_USER_DEFINE_FILE")

# Reference flags
#set(COMPILER_COMMON_FLAGS "${COMPILER_P_PROC_DEFS} ${APP_PREC_DEFS} --specs=nano.specs -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums -Werror -Wall -Wextra -Wuninitialized -Wdouble-promotion -Wno-unused-parameter -fno-builtin -fstrength-reduce")
#   set(COMPILER_COMMON_FLAGS "${COMPILER_P_PROC_DEFS} ${APP_PREC_DEFS} --specs=nano.specs -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums -Wall -Wextra -Wuninitialized -Wdouble-promotion -Wno-unused-parameter -fno-builtin -fstrength-reduce")
#   set(CMAKE_C_FLAGS   "${MCPU_FLAGS} ${VFP_FLAGS} ${COMPILER_COMMON_FLAGS} -std=gnu11 -Wno-long-long -Wconversion -Wno-sign-conversion -pedantic -Werror-implicit-function-declaration -Wstrict-prototypes -fno-cond-mismatch -Werror-implicit-function-declaration -Wstrict-prototypes -fstack-usage -mlong-calls")
#   set(CMAKE_CXX_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} ${COMPILER_COMMON_FLAGS} -c -std=gnu++14  -fno-rtti -fno-use-cxa-atexit -fno-exceptions -fstack-usage -mlong-calls")

set(COMPILER_COMMON_FLAGS "${COMPILER_P_PROC_DEFS} ${APP_PREC_DEFS} --specs=nano.specs -Wall -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS   "${MCPU_FLAGS} ${VFP_FLAGS} ${COMPILER_COMMON_FLAGS} -std=gnu11 ")
set(CMAKE_CXX_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} ${COMPILER_COMMON_FLAGS} -c -std=gnu++14 -fno-rtti")
set(CMAKE_ASM_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} -c -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${MCPU_FLAGS} -T${LINKER_SCRIPT} --specs=nosys.specs -Wl,-Map=FPrime.map -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--gc-sections,-print-memory-usage -Wl,--end-group")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_ASM_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_ASM_FLAGS_DEBUG "-g3")

set(CMAKE_C_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_ASM_FLAGS_RELEASE "")

# Specify paths to root of toolchain package, for searching for libraries, executables, etc.
set(CMAKE_FIND_ROOT_PATH  "${STM32_TOOLS_PATH}")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

