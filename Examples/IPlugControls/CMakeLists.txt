cmake_minimum_required(VERSION 3.16)

# Include the IPlug2.cmake module. Path can be absolute or relative.
include("../../cmake/IPlug2.cmake")

# Declare our project. This example is using name of the declared project as base for other settings
# later on by writing ${PROJECT_NAME} instead of manually writing "IPlugControls" in multiple places.
project(IPlugControls)

# Project configuration. Quotation marks are required if there is a space or semi-colon in the string.
iplug_configure_project(
    PLUG_NAME                   "My Plugin"
    PLUG_NAME_SHORT             IPCT
    PLUG_MFR                    AcmeInc
    PLUG_VERSION_STR            1.0.0
    PLUG_UNIQUE_ID              'Ipct'
    PLUG_MFR_ID                 'Acme'
    PLUG_URL_STR                https://iplug2.github.io
    PLUG_EMAIL_STR              spam@me.com
    PLUG_COPYRIGHT_STR          "Copyright 2020 Acme Inc"
    PLUG_CLASS_NAME             ${PROJECT_NAME}
    PLUG_TYPE                   Instrument
    PLUG_CHANNEL_IO             0-2
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

    BUNDLE_NAME                 ${PROJECT_NAME}
    BUNDLE_DOMAIN               com
    BUNDLE_ICON                 resources/IPlugControls.ico

    # Resource definitions can also be used to add custom string definitions.
    # Each definition will automatically be surrounded by quotation marks.
    # To add normal definitions, use cmake command target_compile_definitions(<target>)
    RESOURCE_DEFINITIONS
        ROBOTO_FN               Roboto-Regular.ttf
        FORK_AWESOME_FN         forkawesome-webfont.ttf
        FONTAUDIO_FN            fontaudio.ttf
        PNGKNOB_FN              knob.png
        PNGKNOB_2X_FN           knob@2x.png
        PNGKNOBROTATE_FN        knob-rotate.png
        PNGKNOBROTATE_2X_FN     knob-rotate@2x.png
        PNGSWITCH_FN            switch.png
        PNGBUTTON_FN            button.png
        PNGSLIDERHANDLE_FN      slider-handle.png
        PNGSLIDERTRACK_FN       slider-track.png
        PNGTEXT_FN              font.png
        PNGTEXT_2X_FN           font@2x.png
        SVGSLIDERHANDLE_FN      slider-handle.svg
        SVGSLIDERTRACK_FN       slider-track.svg
        SVGKNOBROTATE_FN        vector-knob.svg
        SVGHSLIDERHANDLE_FN     hslider-handle.svg
        SVGHSLIDERTRACK_FN      hslider-track.svg

    # Alternative method to automatically add include directories, source files and
    # link libraries to all targets created by iplug_add_<type>(<target name> ...)
    INCLUDE_DIRECTORIES
        PRIVATE
            resources
            resources/fonts
            resources/img

    SOURCES
        # FILTER can be used to specify what files to include with the FROM_DIRECTORY commands
        # Each definition must be in the format of "*.<ext>" and is valid from declaration
        # until end of current scope (PUBLIC, PRIVATE or INTERFACE).
        # Default FILTER, if not specified is:
        # "*.c;*.cpp;*.cxx;*.cc;*.tli;*.tlh;*.h;*.hh;*.hpp;*.hxx;*.hh;*.inl;*.ipp;*.rc;*.resx;*.idl;*.asm;*.inc;*.m;*.mm;*.ico"
        PRIVATE
            FILTER         "*.c;*.cpp;*.h"               # Custom filter for the following FROM_DIRECTORY commands
            FROM_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" # Adds source files from current source directory
            FILTER         *.h  *.rc                     # Changing the filter again for the next FROM_DIRECTORY command.
                           *.ico                         # This time showing an alternative layout.
            FROM_DIRECTORY ./resources                   # Equivalent to ${CMAKE_CURRENT_SOURCE_DIR}/resources

    LINK_LIBRARIES
        PRIVATE
            # additional libraries if needed
)


# Add targets.
iplug_add_application(${PROJECT_NAME}-app   # Using name of the declared project as base for our app target name
    OVERRIDE
        PLUG_NAME     ${PROJECT_NAME}       # Overrides PLUG_NAME set in project configuration for this target

    NUM_CHANNELS        2
    N_VECTOR_WAIT       0
    COPY_AUV3           0
    SIGNAL_VECTOR_SIZE  64
)

iplug_add_vst3(${PROJECT_NAME}-vst3         # Using name of the declared project as base for our vst3 target name
    OVERRIDE
        PLUG_NAME     ${PROJECT_NAME}       # Overrides PLUG_NAME set in project configuration for this target

    EXTENSION           vst3
    SUBCATEGORY         Instrument|Synth
    CC_UNITNAME         "MIDI CCs"
    NUM_CC_CHANS        1
    NUM_MIDI_IN_CHANS   1
    NUM_MIDI_OUT_CHANS  1
    PRESET_LIST         0
)

# iplug_add_aax(${PROJECT_NAME}-aax
#     TYPE_IDS                "'IPCT'"
#     TYPE_IDS_AUDIOSUITE     "'EFA1', 'EFA2'"
#     PLUG_CATEGORY_STR       None
#     DOES_AUDIOSUITE         0
# )


# Generate source list for IDE
get_target_property(SOURCE_LIST ${PROJECT_NAME}-app SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_LIST})
