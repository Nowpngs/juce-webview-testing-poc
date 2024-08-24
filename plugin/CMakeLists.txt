cmake_minimum_required(VERSION 3.22)

project(JuceWebViewPlugin VERSION 0.1.0)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/JuceWebViewPlugin")

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME TestCompany
    IS_SYNTH FALSE
    NEEDS_MIDI_OUTPUT FALSE
    NEEDS_MIDI_INPUT FALSE
    PLUGIN_MANUFACTURER_CODE TEST
    PLUGIN_CODE AUPL
    FORMATS Standalone VST3
    PRODUCT_NAME "JuceWebViewPlugin"
)

get_target_property(PRODUCT_NAME JuceWebViewPlugin JUCE_PRODUCT_NAME)
get_target_property(COMPANY_NAME JuceWebViewPlugin JUCE_COMPANY_NAME)
target_compile_definitions(JuceWebViewPlugin PRIVATE
    JUCE_PRODUCT_NAME=" ${PRODUCT_NAME}"
    JUCE_COMPANY_NAME=" ${COMPANY_NAME}"
    JUCE_PRODUCT_VERSION=" ${PROJECT_VERSION}"
)

target_sources(${PROJECT_NAME}
    PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    ${INCLUDE_DIR}/PluginEditor.h
    ${INCLUDE_DIR}/PluginProcessor.h
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    juce::juce_audio_utils
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../libs/juce/modules/juce_gui_extra/native/javascript/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/ui/public/js/juce/")

if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        target_compile_options(${PROJECT_NAME} PRIVATE /Wall /WX)
        PRIVATE
        _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)