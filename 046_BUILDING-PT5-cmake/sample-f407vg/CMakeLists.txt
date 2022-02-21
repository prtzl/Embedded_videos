# Template CMakeLists.txt configuration for STM32 MCUS
cmake_minimum_required(VERSION 3.12)
# ##############################################################################
# Edit your MCU information up to the next border
project(sample-f407vg)
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PROJECT)

set(MCU_FAMILY STM32F4xx)
set(MCU_MODEL STM32F407xx)
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard)

set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/CubeMX/startup_stm32f407xx.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/CubeMX/STM32F407VGTx_FLASH.ld)
# ##############################################################################
set(EXECUTABLE ${CMAKE_PROJECT_NAME})
enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Headers
set(CUBEMX_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include)
set(PROJECT_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_DIR})

# Sources
file(GLOB_RECURSE STM32CUBEMX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.c)
file(GLOB_RECURSE PROJECT_SOURCES FOLLOW_SYMLINKS
    ${PROJECT_DIR}/*.cpp
    ${PROJECT_DIR}/*.c)

# Executable files
add_executable(${EXECUTABLE}
    ${STM32CUBEMX_SOURCES} 
    ${PROJECT_SOURCES}
    ${STARTUP_SCRIPT})

# Embedded macros(defines)
target_compile_definitions(${EXECUTABLE} PRIVATE
    ${MCU_MODEL}
    USE_HAL_DRIVER)

# Add header directories (AFTER add_executable !!)
target_include_directories(${EXECUTABLE} PRIVATE
    ${CUBEMX_INCLUDE_DIRECTORIES}
    ${PROJECT_INCLUDE_DIRECTORIES})

# Compiler and linker options
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>)

target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -Wl,--end-group
    -Wl,--print-memory-usage)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}>
    ${EXECUTABLE}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}>
    ${EXECUTABLE}.bin)
