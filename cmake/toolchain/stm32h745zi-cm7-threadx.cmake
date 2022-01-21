# Set system name
set(CMAKE_SYSTEM_NAME Nucleo-ThreadX)
set(CMAKE_SYSTEM_VERSION 0.1)
set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
set(CMAKE_CROSSCOMPILING 1)

set(STM_COMPILER_ROOT "${STM32_TOOLS_PATH}")
# Check toolchain directory exists
IF(NOT EXISTS "${STM_COMPILER_ROOT}")
    message(FATAL_ERROR " STM compilers not found at ${STM_COMPILER_ROOT}.")
endif()
message(STATUS "Using STM compilers at: ${STM_COMPILER_ROOT}")

# specify the cross compiler
set(CMAKE_C_COMPILER   "${STM_COMPILER_ROOT}/bin/arm-none-eabi-gcc"         CACHE PATH "gcc"       FORCE)
set(CMAKE_CXX_COMPILER "${STM_COMPILER_ROOT}/bin/arm-none-eabi-g++"         CACHE PATH "g++"       FORCE)
set(CMAKE_ASM_COMPILER "${STM_COMPILER_ROOT}/bin/arm-none-eabi-gcc"         CACHE PATH "assembler" FORCE)
set(CMAKE_AR           "${STM_COMPILER_ROOT}/bin/arm-none-eabi-ar"          CACHE PATH "archive"   FORCE)
set(CMAKE_LINKER       "${STM_COMPILER_ROOT}/bin/arm-none-eabi-ld"          CACHE PATH "linker"    FORCE)
set(CMAKE_NM           "${STM_COMPILER_ROOT}/bin/arm-none-eabi-nm"          CACHE PATH "nm"        FORCE)
set(CMAKE_OBJCOPY      "${STM_COMPILER_ROOT}/bin/arm-none-eabi-objcopy"     CACHE PATH "objcopy"   FORCE)
set(CMAKE_OBJDUMP      "${STM_COMPILER_ROOT}/bin/arm-none-eabi-objdump"     CACHE PATH "objdump"   FORCE)
set(CMAKE_STRIP        "${STM_COMPILER_ROOT}/bin/arm-none-eabi-strip"       CACHE PATH "strip"     FORCE)
set(CMAKE_SIZE         "${STM_COMPILER_ROOT}/bin/arm-none-eabi-size"        CACHE PATH "size"      FORCE)
set(CMAKE_RANLIB       "${STM_COMPILER_ROOT}/bin/arm-none-eabi-ranlib"      CACHE PATH "ranlib"    FORCE)

set(CMAKE_CXX_LINK_EXECUTABLE "${STM32_LD_SCRIPTS_PATH}/CM7/STM32H745ZITX_FLASH.ld -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# MCU and VFP flags
set(MCPU_FLAGS "-mthumb -mcpu=cortex-m7")
set(VFP_FLAGS "-mfpu=fpv5-d16 -mfloat-abi=hard")

# Toolchain preprocessor definitions
set(TOOL_PREC_DEFS "-DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H745xx")
# Application preprocessor definitions
set(APP_PREC_DEFS "-DTX_INCLUDE_USER_DEFINE_FILE -DFX_INCLUDE_USER_DEFINE_FILE")

#set(COMPILER_COMMON_FLAGS "${TOOL_PREC_DEFS} ${APP_PREC_DEFS} --specs=nano.specs -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums -Werror -Wall -Wextra -Wuninitialized -Wdouble-promotion -Wno-unused-parameter -fno-builtin -fstrength-reduce")
set(COMPILER_COMMON_FLAGS "${TOOL_PREC_DEFS} ${APP_PREC_DEFS} --specs=nano.specs -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums -Wall -Wextra -Wuninitialized -Wdouble-promotion -Wno-unused-parameter -fno-builtin -fstrength-reduce")
set(CMAKE_C_FLAGS   "${MCPU_FLAGS} ${VFP_FLAGS} ${COMPILER_COMMON_FLAGS} -std=gnu11 -Wno-long-long -Wconversion -Wno-sign-conversion -pedantic -Werror-implicit-function-declaration -Wstrict-prototypes -fno-cond-mismatch -Werror-implicit-function-declaration -Wstrict-prototypes -fstack-usage -mlong-calls")
set(CMAKE_CXX_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} ${COMPILER_COMMON_FLAGS} -c -std=gnu++14  -fno-rtti -fno-use-cxa-atexit -fno-exceptions -fstack-usage -mlong-calls")
set(CMAKE_ASM_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} -c -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${MCPU_FLAGS} --specs=nosys.specs -Wl,-Map=FPrime.map -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--gc-sections,-print-memory-usage -Wl,--end-group")

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

