cmake_minimum_required(VERSION 3.16)

include("../../cmake/IPlug2.cmake")

project(IPlugEffect)

# Minimalistic configuration using mostly default values
iplug_configure_project(
    PLUG_NAME_SHORT             IPCT
    PLUG_UNIQUE_ID              'Ipef'
    PLUG_CHANNEL_IO             "1-1 2-2"

    RESOURCE_DEFINITIONS
        ROBOTO_FN               Roboto-Regular.ttf

    INCLUDE_DIRECTORIES
        PRIVATE
            resources
            resources/fonts

    SOURCES
        PRIVATE
            FROM_DIRECTORY .
            FROM_DIRECTORY ./resources
)


# Add targets.
iplug_add_application(${PROJECT_NAME}-app)

iplug_add_vst3(${PROJECT_NAME}-vst3
    SUBCATEGORY         Fx
)

# Generate source list for IDE
get_target_property(SOURCE_LIST ${PROJECT_NAME}-app SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_LIST})
