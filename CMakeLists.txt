# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "E:/pico-sdk")

set(PICO_EXTRAS_PATH "E:/pico-extras")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
include(pico_extras_import.cmake)

#include_directories( "pico-extras/src/common/pico_util_buffer/include/pico/util" "pico-extras/src/common/pico_audio/include/pico")

project(pico-fmSynth C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(pico-fmSynth pico-fmSynth.cpp audio_i2s.c
fmSynth/fmSynth_main.cpp
fmSynth/envelope.cpp
fmSynth/oscillator.cpp
fmSynth/fmChannel.cpp
fmSynth/patch.cpp
fmSynth/miditones.cpp
fmSynth/midiFreqTable.c
sampleMidi.c)

pico_generate_pio_header(pico-fmSynth ${CMAKE_CURRENT_LIST_DIR}/audio_i2s.pio)

pico_set_program_name(pico-fmSynth "pico-fmSynth")
pico_set_program_version(pico-fmSynth "0.3_interp")

pico_enable_stdio_uart(pico-fmSynth 1)
pico_enable_stdio_usb(pico-fmSynth 0)

#target_compile_options(pico-fmSynth PRIVATE -H)

target_compile_definitions(pico-fmSynth PRIVATE PICO_AUDIO_I2S_MONO_INPUT)

# Add the standard library to the build
target_link_libraries(pico-fmSynth
        pico_audio
        pico_multicore        
        pico_stdlib
        )

# Add any user requested libraries
target_link_libraries(pico-fmSynth
        hardware_dma
        hardware_pio
        hardware_interp
        )

pico_add_extra_outputs(pico-fmSynth)
