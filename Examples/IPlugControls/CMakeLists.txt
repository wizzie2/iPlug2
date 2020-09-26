cmake_minimum_required(VERSION 3.17)

# Include the IPlug2.cmake module. Path can be absolute or relative.
include("../../cmake/IPlug2.cmake")

# Declare our project
project(IPlugControls)

# Project configuration
iplug_configure_project(
    BUNDLE_NAME                 "IPlugControls"
    BUNDLE_DOMAIN               "com"
    BUNDLE_ICON                 "resources/IPlugControls.ico"

    PLUG_CLASS_NAME             IPlugControls
    PLUG_NAME                   "IPlugControls"
    PLUG_NAME_SHORT             "IPCT"
    PLUG_MFR                    "AcmeInc"
    PLUG_VERSION_HEX            0x00010000
    PLUG_VERSION_STR            "1.0.0"
    PLUG_UNIQUE_ID              'Ipct'
    PLUG_MFR_ID                 'Acme'
    PLUG_URL_STR                "https://iplug2.github.io"
    PLUG_EMAIL_STR              "spam@me.com"
    PLUG_COPYRIGHT_STR          "Copyright 2020 Acme Inc"
    PLUG_TYPE                   Instrument
    PLUG_CHANNEL_IO             "0-2"
    PLUG_LATENCY                0
    PLUG_DOES_MIDI_IN           1
    PLUG_DOES_MIDI_OUT          0
    PLUG_DOES_MPE               0
    PLUG_DOES_STATE_CHUNKS      0
    PLUG_HAS_UI                 1
    PLUG_WIDTH                  1024
    PLUG_HEIGHT                 768
    PLUG_HOST_RESIZE            0
    PLUG_FPS                    60
    PLUG_SHARED_RESOURCES       0

    RESOURCE_DEFINITIONS
        ROBOTO_FN               "Roboto-Regular.ttf"
        FORK_AWESOME_FN         "forkawesome-webfont.ttf"
        FONTAUDIO_FN            "fontaudio.ttf"
        PNGKNOB_FN              "knob.png"
        PNGKNOB_2X_FN           "knob@2x.png"
        PNGKNOBROTATE_FN        "knob-rotate.png"
        PNGKNOBROTATE_2X_FN     "knob-rotate@2x.png"
        PNGSWITCH_FN            "switch.png"
        PNGBUTTON_FN            "button.png"
        PNGSLIDERHANDLE_FN      "slider-handle.png"
        PNGSLIDERTRACK_FN       "slider-track.png"
        PNGTEXT_FN              "font.png"
        PNGTEXT_2X_FN           "font@2x.png"
        SVGSLIDERHANDLE_FN      "slider-handle.svg"
        SVGSLIDERTRACK_FN       "slider-track.svg"
        SVGKNOBROTATE_FN        "vector-knob.svg"
        SVGHSLIDERHANDLE_FN     "hslider-handle.svg"
        SVGHSLIDERTRACK_FN      "hslider-track.svg"
)


set(SOURCE_LIST
    PRIVATE
        "IconsFontaudio.h"
        "IconsForkAwesome.h"
        "IPlugControls.cpp"
        "IPlugControls.h"
        "resources/main.rc"
        "resources/IPlugControls.ico"
)

set(INCLUDE_DIRECTORIES
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        "resources"
        "resources/fonts"
        "resources/img"
)

# Add targets
iplug_add_application(IPlugControls-app
    NUM_CHANNELS            2
    N_VECTOR_WAIT           0
    COPY_AUV3               0
    SIGNAL_VECTOR_SIZE      64
)

iplug_add_vst3(IPlugControls-vst3
    EXTENSION               vst3
    SUBCATEGORY             Instrument|Synth
)

# iplug_add_aax(IPlugControls-aax
#     AAX_TYPE_IDS                "'IPCT'"
#     AAX_TYPE_IDS_AUDIOSUITE     "'EFA1', 'EFA2'"
#     AAX_PLUG_CATEGORY_STR       None
#     AAX_DOES_AUDIOSUITE         0
# )

target_sources(IPlugControls-app ${SOURCE_LIST})
target_sources(IPlugControls-vst3 ${SOURCE_LIST})

target_include_directories(IPlugControls-app PRIVATE ${INCLUDE_DIRECTORIES})
target_include_directories(IPlugControls-vst3 PRIVATE ${INCLUDE_DIRECTORIES})

# extra libraries to link if needed
target_link_libraries(IPlugControls-app PRIVATE )
target_link_libraries(IPlugControls-vst3 PRIVATE )

# Generate source list for IDE
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_LIST})
