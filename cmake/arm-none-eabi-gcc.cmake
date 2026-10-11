# Minimal cross toolchain file for ARM bare metal
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)


# Toolchain (adjust if your executable names differ)
set(TOOLCHAIN_PREFIX arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}-size)


# Prevent try-compile from running for an executable
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)