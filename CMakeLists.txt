# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# ================ CONFIGURAÇÃO DO SDK - ESSENCIAL ================
# Esta linha DEVE vir antes do project()
include(pico_sdk_import.cmake)



project(blink C CXX ASM)

# ================ INICIALIZAÇÃO DO SDK - ESSENCIAL ================
pico_sdk_init()

# ================ EXECUTÁVEL PRINCIPAL ================
add_executable(blink 
    blink.c 
    inc/ssd1306.c
)

# ================ CONFIGURAÇÃO PIO ================
pico_generate_pio_header(blink ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

# ================ CONFIGURAÇÕES DO PROJETO ================
pico_set_program_name(blink "teste")
pico_set_program_version(blink "0.1")

pico_enable_stdio_usb(blink 1)
pico_enable_stdio_uart(blink 0)

target_link_libraries(blink
    pico_stdlib
    hardware_i2c
    hardware_pio
    hardware_irq
    hardware_dma
)

target_include_directories(blink PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

pico_add_extra_outputs(blink)
