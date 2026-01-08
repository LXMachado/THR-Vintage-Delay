cmake_minimum_required(VERSION 3.15)

project(BlueHaze VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_DIR "/Users/alexandremachado/JUCE")
add_subdirectory(${JUCE_DIR} JUCE)

juce_add_plugin(BlueHaze
    COMPANY_NAME "Tinnie House DSP"
    PRODUCT_NAME "Blue Haze"
    PLUGIN_MANUFACTURER_CODE THdP
    PLUGIN_CODE BHaz
    FORMATS VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(BlueHaze PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Params.h
    Source/VintageDelayDSP.h
)

target_compile_definitions(BlueHaze PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

target_link_libraries(BlueHaze PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_basics
)
