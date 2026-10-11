cmake_minimum_required(VERSION 3.20)

# Toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/arm-none-eabi-gcc.cmake)

project(P003_baremetal C ASM)

# Target
add_executable(P003_baremetal
    src/startup_stm32f072.s
    src/main.c
)

target_include_directories(P003_baremetal 
    PRIVATE 
        inc
)

# MCU / CPU (STM32F0 = Cortex-M0)
set(MCU_OPTIONS
    -mcpu=cortex-m0
    -mthumb
)

# Compile options
target_compile_options(P003_baremetal PRIVATE ${MCU_OPTIONS})

set(C_ONLY_OPTIONS
    -O2
    -ffunction-sections
    -fdata-sections
    -fno-builtin
    -fno-exceptions
    -Wall
    -Wextra
)
target_compile_options(P003_baremetal PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_ONLY_OPTIONS}>
)

# Link options
set(LINK_OPTIONS
    ${MCU_OPTIONS}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/P003_baremetal.map
    -T${CMAKE_SOURCE_DIR}/linker/stm32f072.ld
    -nostartfiles
    -nostdlib
)
target_link_options(P003_baremetal PRIVATE ${LINK_OPTIONS})

# Link libc minimal (only if needed). For this blink we can omit.
# target_link_libraries(P003_baremetal m c gcc)

# Post-build: .bin and .hex + size
add_custom_command(TARGET P003_baremetal POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:P003_baremetal> ${CMAKE_BINARY_DIR}/P003_baremetal.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:P003_baremetal> ${CMAKE_BINARY_DIR}/P003_baremetal.hex
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:P003_baremetal>
    BYPRODUCTS ${CMAKE_BINARY_DIR}/P003_baremetal.bin ${CMAKE_BINARY_DIR}/P003_baremetal.hex
    COMMENT "Objcopy + size"
)

# Convenience flash target using STM32_Programmer_CLI
add_custom_target(flash
    COMMAND STM32_Programmer_CLI -c port=SWD freq=950 sn=066EFF485570854967101553 -w ${CMAKE_BINARY_DIR}/P003_baremetal.bin 0x08000000 -v -rst
    DEPENDS P003_baremetal
    COMMENT "Flashing with STM32_Programmer_CLI"
)