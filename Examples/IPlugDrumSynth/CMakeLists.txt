cmake_minimum_required(VERSION 3.16)

# Include the IPlug2.cmake module. Path can be absolute or relative.
include("../../cmake/IPlug2.cmake")

# Declare our project
project(IPlugDrumSynth)

# Project configuration
iplug_configure_project(
    PLUG_NAME                   "IPlugDrumSynth"
    PLUG_NAME_SHORT             "IPDS"
    PLUG_MFR                    "AcmeInc"
    PLUG_VERSION_HEX            0x00010000
    PLUG_VERSION_STR            "1.0.0"
    PLUG_UNIQUE_ID              'GZDU'
    PLUG_MFR_ID                 'Acme'
    PLUG_URL_STR                "https://iplug2.github.io"
    PLUG_EMAIL_STR              "spam@me.com"
    PLUG_COPYRIGHT_STR          "Copyright 2020 Acme Inc"
    PLUG_CLASS_NAME             IPlugDrumSynth

    BUNDLE_NAME                 "IPlugDrumSynth"
    BUNDLE_DOMAIN               "com"
    BUNDLE_ICON                 "resources/IPlugDrumSynth.ico"

    PLUG_CHANNEL_IO             "0-2 0-2.2 0-2.2.2 0-2.2.2.2"

    PLUG_LATENCY                0
    PLUG_TYPE                   Instrument
    PLUG_DOES_MIDI_IN           1
    PLUG_DOES_MIDI_OUT          1
    PLUG_DOES_MPE               0
    PLUG_DOES_STATE_CHUNKS      0
    PLUG_HAS_UI                 1
    PLUG_WIDTH                  335
    PLUG_HEIGHT                 335
    PLUG_FPS                    60
    PLUG_SHARED_RESOURCES       0
    PLUG_HOST_RESIZE            0

    RESOURCE_DEFINITIONS
        ROBOTO_FN               "Roboto-Regular.ttf"

    INCLUDE_DIRECTORIES
        PRIVATE
            "resources"
            "resources/fonts"

    SOURCES
        PRIVATE
            "IPlugDrumSynth.cpp"
            "IPlugDrumSynth.h"
            "IPlugDrumSynth_DSP.h"
            "resources/main.rc"
            "resources/IPlugDrumSynth.ico"
)


# Add targets
iplug_add_application(IPlugDrumSynth-app
    OVERRIDE
        PLUG_CHANNEL_IO             "0-2"

    NUM_CHANNELS        2
    N_VECTOR_WAIT       0
    COPY_AUV3           0
    SIGNAL_VECTOR_SIZE  64
)

iplug_add_vst3(IPlugDrumSynth-vst3
    EXTENSION               vst3
    SUBCATEGORY             Instrument|Synth
)

# iplug_add_aax(IPlugDrumSynth-aax
#     TYPE_IDS 'IPD1'
#     PLUG_MFR_STR "Acme"
#     DOES_AUDIOSUITE 0
#     PLUG_CATEGORY_STR "Synth"
#     AOS_STRS "Drum2", "Drum3", "Drum4"
# )

# Generate source list for IDE
get_target_property(SOURCE_LIST IPlugDrumSynth-app SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_LIST})
