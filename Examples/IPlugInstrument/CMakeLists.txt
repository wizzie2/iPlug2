cmake_minimum_required(VERSION 3.16)

# Include the IPlug2.cmake module. Path can be absolute or relative.
include("../../cmake/IPlug2.cmake")

# Declare our project
project(IPlugInstrument)

# Project configuration
iplug_configure_project(
    PLUG_NAME                   "IPlugInstrument"
    PLUG_NAME_SHORT             "IPIN"
    PLUG_MFR                    "AcmeInc"
    PLUG_VERSION_STR            "1.0.0"
    PLUG_UNIQUE_ID              'PmBl'
    PLUG_MFR_ID                 'Acme'
    PLUG_URL_STR                "https://iplug2.github.io"
    PLUG_EMAIL_STR              "spam@me.com"
    PLUG_COPYRIGHT_STR          "Copyright 2020 Acme Inc"

    BUNDLE_NAME                 "IPlugInstrument"
    BUNDLE_DOMAIN               "com"
    BUNDLE_ICON                 "resources/IPlugInstrument.ico"

    PLUG_CHANNEL_IO             "0-2"
    PLUG_LATENCY                0
    PLUG_TYPE                   Instrument
    PLUG_DOES_MIDI_IN           1
    PLUG_DOES_MIDI_OUT          1
    PLUG_DOES_MPE               1
    PLUG_WIDTH                  1024
    PLUG_HEIGHT                 669
    PLUG_FPS                    60
    PLUG_HOST_RESIZE            0

    RESOURCE_DEFINITIONS
        ROBOTO_FN               "Roboto-Regular.ttf"

    INCLUDE_DIRECTORIES
        PRIVATE
            "resources"
            "resources/fonts"

    SOURCES
        PRIVATE
            FROM_DIRECTORY .
            FROM_DIRECTORY ./resources
)

# Add targets
iplug_add_application(${PROJECT_NAME}-app)
iplug_add_vst3(${PROJECT_NAME}-vst3 SUBCATEGORY Instrument|Synth)

# Generate source list for IDE
get_target_property(SOURCE_LIST ${PROJECT_NAME}-app SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_LIST})
