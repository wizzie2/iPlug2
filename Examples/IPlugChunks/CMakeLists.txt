cmake_minimum_required(VERSION 3.16)

# Include the IPlug2.cmake module. Path can be absolute or relative.
include("../../cmake/IPlug2.cmake")

# Declare our project
project(IPlugChunks)

# Project configuration
iplug_configure_project(
    BUNDLE_NAME                 "IPlugChunks"
    BUNDLE_DOMAIN               "com"

    PLUG_CLASS_NAME             IPlugChunks
    PLUG_NAME                   "IPlugChunks"
    PLUG_NAME_SHORT             "IPCH"
    PLUG_MFR                    "AcmeInc"
    PLUG_VERSION_STR            "1.0.0"
    PLUG_UNIQUE_ID              'W1Q1'
    PLUG_MFR_ID                 'Acme'
    PLUG_URL_STR                "https://iplug2.github.io"
    PLUG_EMAIL_STR              "spam@me.com"
    PLUG_COPYRIGHT_STR          "Copyright 2020 Acme Inc"
    PLUG_CHANNEL_IO             "1-1 2-2"
    PLUG_LATENCY                0
    PLUG_TYPE                   Effect
    PLUG_DOES_MIDI_IN           0
    PLUG_DOES_MIDI_OUT          0
    PLUG_DOES_MPE               0
    PLUG_DOES_STATE_CHUNKS      1
    PLUG_HAS_UI                 1
    PLUG_WIDTH                  600
    PLUG_HEIGHT                 600
    PLUG_FPS                    60
    PLUG_SHARED_RESOURCES       0
    PLUG_HOST_RESIZE            0

    RESOURCE_DEFINITIONS
        ROBOTO_FN               "Roboto-Regular.ttf"
)


set(SOURCE_LIST
    PRIVATE
        "IPlugChunks.cpp"
        "IPlugChunks.h"
        "resources/main.rc"
        "resources/IPlugChunks.ico"
)

set(INCLUDE_DIRECTORIES
    PRIVATE
        "resources"
        "resources/fonts"
        "resources/img"
)

# Add targets
iplug_add_application(IPlugChunks-app
    NUM_CHANNELS        2
    N_VECTOR_WAIT       0
    COPY_AUV3           0
    SIGNAL_VECTOR_SIZE  64
)

iplug_add_vst3(IPlugChunks-vst3
    SUBCATEGORY         Fx
)

# iplug_add_aax(IPlugChunks-aax
#     TYPE_IDS                "'IEF1', 'IEF2'"
#     TYPE_IDS_AUDIOSUITE     "'IEA1', 'IEA2'"
#     PLUG_CATEGORY_STR       "Effect"
#     DOES_AUDIOSUITE         1
# )

target_sources(IPlugChunks-app ${SOURCE_LIST})
target_sources(IPlugChunks-vst3 ${SOURCE_LIST})

target_include_directories(IPlugChunks-app PRIVATE ${INCLUDE_DIRECTORIES})
target_include_directories(IPlugChunks-vst3 PRIVATE ${INCLUDE_DIRECTORIES})

# extra libraries to link if needed
target_link_libraries(IPlugChunks-app PRIVATE )
target_link_libraries(IPlugChunks-vst3 PRIVATE )

# Generate source list for IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

