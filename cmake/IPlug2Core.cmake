include_guard(GLOBAL)

#------------------------------------------------------------------------------
# git_update_submodules
# Updates all IPlug2 repository submodules

# function(iplug_git_update_submodules)
#     # Get/Set IPLUG2_UPDATE_GIT_SUBMODULES cache option
#     option(IPLUG2_UPDATE_GIT_SUBMODULES "Automatically update git submodules at generation." OFF)

#     # return if we're not updating submodules
#     if(NOT IPLUG2_UPDATE_GIT_SUBMODULES)
#         return()
#     endif()

#     if(GIT_FOUND AND EXISTS "${IPLUG2_ROOT_PATH}/.git")
#         execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                         COMMAND_ECHO STDOUT
#                         RESULT_VARIABLE GIT_SUBMOD_RESULT)
#         if(NOT GIT_SUBMOD_RESULT EQUAL "0")
#             message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
#         endif()
#     endif()
# endfunction()


#------------------------------------------------------------------------------

function(iplug_get_version _file _outvar)
    # Get IPlug2 verion number from /IPlug/IPlugVersion.h
    # This will make sure that the version number is correct in the source
    # without regenerating every time it changes

    # TODO: change to "^#define IPLUG_VERSION[ \t]+([0-9]+)\\.([0-9]+)\\.([0-9]+)$" to get
    # x.x.x matches to \\1 \\2 \\3 instead of 3 different defines for major, minor and patch

    set(_version_regex "#define IPLUG_VERSION_.*[ \t]+(.+)")
    file(STRINGS "${_file}" _iplug_version REGEX ${_version_regex})
    list(TRANSFORM _iplug_version REPLACE ${_version_regex} "\\1")
    string(JOIN "." _iplug_version ${_iplug_version})
    set(${_outvar} ${_iplug_version} PARENT_SCOPE)
endfunction()


#------------------------------------------------------------------------------
# iplug_configure_project

macro(iplug_configure_project)
    _iplug_check_initialized()
    set(_caller_override iplug_configure_project)

    string_assert(${PROJECT_NAME} "PROJECT_NAME is empty. Make sure to call configure after project declarations.")

    set(_multiValueArgs
        "RESOURCE_DEFINITIONS"
        "INCLUDE_DIRECTORIES"
        "SOURCES"
        "LINK_LIBRARIES"
    )

    set(CONFIG_VARIABLES "")
    set(CONFIG_INCLUDE_DIRECTORIES "")
    set(CONFIG_SOURCES "")
    set(CONFIG_LINK_LIBRARIES "")

    set(DEFAULT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/${PROJECT_NAME}.ico")
    if(NOT EXISTS "${DEFAULT_ICON}")
        set(DEFAULT_ICON "")
    endif()

    cmake_parse_arguments(_arg "" "${_projectConfigArgs}" "${_multiValueArgs}" ${ARGN})
    _iplug_warn_unparsed_arguments("" _arg_UNPARSED_ARGUMENTS)

    foreach(_var IN LISTS _projectConfigArgs)
        _iplug_add_config_variable(CONFIG "" ${_var} "${_arg_${_var}}")
    endforeach()

    cmake_parse_arguments(_sources  "" "" "PUBLIC;PRIVATE;INTERFACE" ${_arg_SOURCES})
    if(DEFINED _sources_UNPARSED_ARGUMENTS AND NOT DEFINED _sources_PUBLIC AND NOT DEFINED _sources_PRIVATE AND NOT DEFINED _sources_INTERFACE)
        iplug_syntax_error("[SOURCES] called with invalid arguments. Missing <PUBLIC|PRIVATE|INTERFACE> declaration.")
    endif()
    set(_sources "")
    foreach(_item IN ITEMS INTERFACE PRIVATE PUBLIC)
        set(_list ${_sources_${_item}})
        if(_list)
            set(SOURCES_FILTER "*.c;*.cpp;*.cxx;*.cc;*.tli;*.tlh;*.h;*.hh;*.hpp;*.hxx;*.hh;*.inl;*.ipp;*.rc;*.resx;*.idl;*.asm;*.inc;*.m;*.mm;*.ico")
            list(APPEND _sources ${_item})
            list(LENGTH _list _len)
            while(_len GREATER 0)
                list(POP_FRONT _list _item_name)
                string(TOUPPER "${_item_name}" _str)
                if("${_str}" STREQUAL "FILTER")
                    unset(_filter)
                    while(TRUE)
                        list(LENGTH _list _len)
                        if(_len LESS 1)
                            break()
                        endif()
                        list(GET _list 0 _next_filter_item)
                        if(NOT _next_filter_item MATCHES "^\\*\\.")
                            break()
                        endif()
                        list(POP_FRONT _list _filter_item)
                        list(APPEND _filter ${_filter_item})
                    endwhile()
                    if(DEFINED _filter)
                        set(SOURCES_FILTER ${_filter})
                    endif()
                elseif("${_str}" STREQUAL "FROM_DIRECTORY" OR "${_str}" STREQUAL "FROM_DIRECTORY_RECURSIVE")
                    set(_expr ${SOURCES_FILTER})
                    list(POP_FRONT _list FROM_DIRECTORY)
                    iplug_validate_string(FROM_DIRECTORY NOTEMPTY PATH_EXISTS)
                    get_filename_component(_file "${FROM_DIRECTORY}" ABSOLUTE)
                    string(REPLACE "*." "${_file}/*." _file "${_expr}")
                    if("${_str}" STREQUAL "FROM_DIRECTORY")
                        file(GLOB _file_sources CONFIGURE_DEPENDS ${_file})
                    elseif("${_str}" STREQUAL "FROM_DIRECTORY_RECURSIVE")
                        file(GLOB_RECURSE _file_sources CONFIGURE_DEPENDS ${_file})
                        string(REGEX REPLACE "${CMAKE_CURRENT_BINARY_DIR}[^;]+;?" "" _file_sources "${_file_sources}")
                    endif()
                    list(APPEND _sources "${_file_sources}")
                else()
                    list(APPEND _sources "${_item_name}")
                endif()
                list(LENGTH _list _len)
            endwhile()
        endif()
    endforeach()

    cmake_parse_arguments(_linklibs  "" "" "PUBLIC;PRIVATE;INTERFACE" ${_arg_LINK_LIBRARIES})
    if(DEFINED _linklibs_UNPARSED_ARGUMENTS AND NOT DEFINED _linklibs_PUBLIC AND NOT DEFINED _linklibs_PRIVATE AND NOT DEFINED _linklibs_INTERFACE)
        iplug_syntax_error("[LINK_LIBRARIES] called with invalid arguments. Missing <PUBLIC|PRIVATE|INTERFACE> declaration.")
    endif()
    set(_linklibs "")
    foreach(_item IN ITEMS INTERFACE PRIVATE PUBLIC)
        if(_linklibs_${_item})
            list(APPEND _linklibs ${_item})
            list(APPEND _linklibs ${_linklibs_${_item}})
        endif()
    endforeach()

    cmake_parse_arguments(_includes  "" "" "PUBLIC;PRIVATE;INTERFACE" ${_arg_INCLUDE_DIRECTORIES})
    if(DEFINED _includes_UNPARSED_ARGUMENTS AND NOT DEFINED _includes_PUBLIC AND NOT DEFINED _includes_PRIVATE AND NOT DEFINED _includes_INTERFACE)
        iplug_syntax_error("[INCLUDE_DIRECTORIES] called with invalid arguments. Missing <PUBLIC|PRIVATE|INTERFACE> declaration.")
    endif()
    set(_includes "")
    foreach(_item IN ITEMS INTERFACE PRIVATE PUBLIC)
        if(_includes_${_item})
            list(APPEND _includes ${_item})
            list(APPEND _includes ${_includes_${_item}})
        endif()
    endforeach()

    _iplug_add_config_variable(CONFIG "" INCLUDE_DIRECTORIES "${_includes}")
    _iplug_add_config_variable(CONFIG "" SOURCES             "${_sources}")
    _iplug_add_config_variable(CONFIG "" LINK_LIBRARIES      "${_linklibs}")

    # resource definitions are always surrounded by quotation marks
    list(LENGTH _arg_RESOURCE_DEFINITIONS _len)
    while(_len GREATER 0)
        list(POP_FRONT _arg_RESOURCE_DEFINITIONS _item_name)
        list(POP_FRONT _arg_RESOURCE_DEFINITIONS _item_val)
        _iplug_add_config_variable(CONFIG_RESOURCE RESOURCE ${_item_name} "\"${_item_val}\"")
        list(LENGTH _arg_RESOURCE_DEFINITIONS _len)
    endwhile()

    # Check validity of config variables.
    _iplug_validate_config_variables(CONFIG)

    # additional variables that are based on existing variables
    _iplug_add_config_variable(CONFIG "" BUNDLE_MFR "${CONFIG_PLUG_MFR}")

    # Generate and add PLUG_VERSION_HEX from PLUG_VERSION_STR
    # Yes, we could have done an ordinary replace dots with semi-colons, but thats no fun.
    string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\1;\\2;\\3" _str ${CONFIG_PLUG_VERSION_STR})
    list(POP_FRONT _str PLUG_VERSION_STR_MAJOR_NUMBER)
    list(POP_FRONT _str PLUG_VERSION_STR_MINOR_NUMBER)
    list(POP_FRONT _str PLUG_VERSION_STR_PATCH_NUMBER)
    iplug_validate_string(PLUG_VERSION_STR_MAJOR_NUMBER MINVALUE 0 MAXVALUE 65535)
    iplug_validate_string(PLUG_VERSION_STR_MINOR_NUMBER MINVALUE 0 MAXVALUE 255)
    iplug_validate_string(PLUG_VERSION_STR_PATCH_NUMBER MINVALUE 0 MAXVALUE 255)
    math(EXPR _VERSION_HEX "(${PLUG_VERSION_STR_MAJOR_NUMBER} << 16) +
                            (${PLUG_VERSION_STR_MINOR_NUMBER} << 8) +
                            (${PLUG_VERSION_STR_PATCH_NUMBER})"
                            OUTPUT_FORMAT HEXADECIMAL)
    _iplug_add_config_variable(CONFIG "" PLUG_VERSION_HEX "${_VERSION_HEX}")

    set(_caller_override "")

    # IPlug2_BINARY_DIR is declared in IPlug2/CMakeLists.txt.
    # If we're adding IPlug with add_subdirectory it never gets declared since we're using
    # a external project as superproject and hitchhiking into that binary directory instead.
    if(NOT IPlug2_BINARY_DIR)
        set(IPLUG2_EXTERNAL_PROJECT TRUE)
        add_subdirectory("${IPLUG2_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/IPlug2")
    endif()

    _iplug_generate_source_groups()
endmacro()


#------------------------------------------------------------------------------
# iplug_add_aax

macro(iplug_add_aax _target)
    iplug_syntax_error("iplug_add_aax() is not implemented.")

    set(_oneValueArgs
        "TYPE_IDS"
        "TYPE_IDS_AUDIOSUITE"
        "PLUG_CATEGORY_STR"
        "DOES_AUDIOSUITE"
    )
    _iplug_parse_target_arguments(CONFIG_AAX AAX "" "${_oneValueArgs}" "" "${ARGN}")

    _iplug_add_config_variable(CONFIG_AAX TARGET_AAX API 1)

    iplug_validate_string(TYPE_IDS             NOTEMPTY PREFIX CONFIG_AAX ALPHA NUMERIC SPACE APOSTROPHE COMMA)
    iplug_validate_string(TYPE_IDS_AUDIOSUITE  NOTEMPTY PREFIX CONFIG_AAX ALPHA NUMERIC SPACE APOSTROPHE COMMA)
    iplug_validate_string(DOES_AUDIOSUITE      DEFAULT "0" PREFIX CONFIG_AAX NOTEMPTY STREQUAL 0 1)
    iplug_validate_string(PLUG_CATEGORY_STR    DEFAULT "None" PREFIX CONFIG_AAX NOTEMPTY STREQUAL None EQ Dynamics PitchShift Reverb Delay Modulation Harmonic NoiseReduction Dither SoundField Effect)

    _iplug_add_config_variable(CONFIG_AAX TARGET_AAX PLUG_MFR_STR  "${CONFIG_PLUG_MFR}")
    _iplug_add_config_variable(CONFIG_AAX TARGET_AAX PLUG_NAME_STR "${CONFIG_PLUG_CLASS_NAME}\\n${CONFIG_PLUG_NAME_SHORT}")
endmacro()


#------------------------------------------------------------------------------
# iplug_add_auv2

macro(iplug_add_auv2 _target)
    iplug_syntax_error("iplug_add_auv2() is not implemented.")

    set(_oneValueArgs
    )
    _iplug_parse_target_arguments(CONFIG_AUV2 AUV2 "" "${_oneValueArgs}" "" "${ARGN}")

    _iplug_add_config_variable(CONFIG_AUV2 TARGET_AUV2 API 1)
    _iplug_add_config_variable(CONFIG_AUV2 TARGET_AUV2 ENTRY           "${CONFIG_PLUG_CLASS_NAME}_Entry")
    _iplug_add_config_variable(CONFIG_AUV2 TARGET_AUV2 ENTRY_STR       "${CONFIG_AUV2_ENTRY}")
    _iplug_add_config_variable(CONFIG_AUV2 TARGET_AUV2 VIEW_CLASS      "${CONFIG_PLUG_CLASS_NAME}_View")
    _iplug_add_config_variable(CONFIG_AUV2 TARGET_AUV2 VIEW_CLASS_STR  "${CONFIG_AUV2_VIEW_CLASS}")
    _iplug_add_config_variable(CONFIG_AUV2 TARGET_AUV2 FACTORY         "${CONFIG_PLUG_CLASS_NAME}_Factory")

    iplug_validate_string(ENTRY      PREFIX CONFIG_AUV2 ALPHAFIRST ALPHA NUMERIC UNDERSCORE)
    iplug_validate_string(VIEW_CLASS PREFIX CONFIG_AUV2 ALPHAFIRST ALPHA NUMERIC UNDERSCORE)
    iplug_validate_string(FACTORY    PREFIX CONFIG_AUV2 ALPHAFIRST ALPHA NUMERIC UNDERSCORE)
endmacro()


#------------------------------------------------------------------------------
# iplug_add_auv3

macro(iplug_add_auv3 _target)
    iplug_syntax_error("iplug_add_auv3() is not implemented.")
    set(_oneValueArgs
    )
    _iplug_parse_target_arguments(CONFIG_AUV3 AUV3 "" "${_oneValueArgs}" "" "${ARGN}")
    _iplug_add_config_variable(CONFIG_AUV3 TARGET_AUV3 API 1)
endmacro()

#------------------------------------------------------------------------------
# iplug_add_web

macro(iplug_add_web _target)
    iplug_syntax_error("iplug_add_web() is not implemented.")
    set(_oneValueArgs
    )
    _iplug_parse_target_arguments(CONFIG_WEB WEB "" "${_oneValueArgs}" "" "${ARGN}")

    _iplug_add_config_variable(CONFIG_WEB TARGET_WEB API 1)
    _iplug_add_config_variable(CONFIG TARGET USE_WEBGL2 0)
    _iplug_add_config_variable(CONFIG TARGET FULL_ES3 1)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_EDITOR 1)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_DSP    0)
endmacro()

#------------------------------------------------------------------------------
# iplug_add_wam

macro(iplug_add_wam _target)
    iplug_syntax_error("iplug_add_wam() is not implemented.")
    set(_oneValueArgs
    )
    _iplug_parse_target_arguments(CONFIG_WAM WAM "" "${_oneValueArgs}" "" "${ARGN}")

    _iplug_add_config_variable(CONFIG_WAM TARGET_WAM API 1)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_EDITOR 0)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_DSP    1)
endmacro()

#------------------------------------------------------------------------------
# iplug_add_vst3c

macro(iplug_add_vst3c _target)
    iplug_syntax_error("iplug_add_vst3c() is not implemented.")
    set(_oneValueArgs
    )
    _iplug_parse_target_arguments(CONFIG_VST3C VST3 "" "${_oneValueArgs}" "" "${ARGN}")

    _iplug_add_config_variable(CONFIG_VST3C TARGET_VST3C API 1)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_EDITOR 1)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_DSP    0)
endmacro()

#------------------------------------------------------------------------------
# iplug_add_vst3p

macro(iplug_add_vst3p _target)
    iplug_syntax_error("iplug_add_vst3p() is not implemented.")
    set(_oneValueArgs
    )
    _iplug_parse_target_arguments(CONFIG_VST3P VST3 "" "${_oneValueArgs}" "" "${ARGN}")

    _iplug_add_config_variable(CONFIG_VST3P TARGET_VST3P API 1)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_EDITOR 0)
    _iplug_add_config_variable(CONFIG_OVERRIDE OVERRIDE IPLUG_DSP    1)
endmacro()

#------------------------------------------------------------------------------
# iplug_add_vst2

macro(iplug_add_vst2 _target)
    iplug_syntax_error("iplug_add_vst2() is not implemented.")
    set(_oneValueArgs
    )
    _iplug_parse_target_arguments(CONFIG_VST2 VST2 "" "${_oneValueArgs}" "" "${ARGN}")
    _iplug_add_config_variable(CONFIG_VST2 TARGET_VST2 API 1)
endmacro()


#------------------------------------------------------------------------------
# iplug_add_application

macro(iplug_add_application _target)
    set(_oneValueArgs
        "SUBSYSTEM"
        "NUM_CHANNELS"
        "N_VECTOR_WAIT"
        "COPY_AUV3"
        "SIGNAL_VECTOR_SIZE"
    )
    _iplug_parse_target_arguments(CONFIG_APP TARGET_APP "" "${_oneValueArgs}" "" "${ARGN}")

    _iplug_add_config_variable(CONFIG_APP TARGET_APP API 1)
    _iplug_add_config_variable(CONFIG_APP TARGET_APP MULT 1)  # APP_MULT should probably be removed

    if(PLATFORM_MAC)
        _iplug_add_config_variable(CONFIG TARGET __MACOSX_CORE__ 1)
        _iplug_add_config_variable(CONFIG TARGET SWELL_COMPILED 1)
    endif()

    string(TOUPPER "${CONFIG_APP_SUBSYSTEM}" CONFIG_APP_SUBSYSTEM)

    iplug_validate_string(SUBSYSTEM            DEFAULT "GUI" PREFIX CONFIG_APP STREQUAL GUI CONSOLE)
    iplug_validate_string(NUM_CHANNELS         DEFAULT "2" PREFIX CONFIG_APP NOTEMPTY NUMERIC)
    iplug_validate_string(N_VECTOR_WAIT        DEFAULT "0" PREFIX CONFIG_APP NOTEMPTY NUMERIC)
    iplug_validate_string(COPY_AUV3            DEFAULT "0" PREFIX CONFIG_APP NOTEMPTY STREQUAL 0 1)
    iplug_validate_string(SIGNAL_VECTOR_SIZE   DEFAULT "64" PREFIX CONFIG_APP NOTEMPTY NUMERIC MAXLENGTH 6)

    add_executable(${_target})
    if(PLATFORM_WINDOWS AND "${CONFIG_APP_SUBSYSTEM}" STREQUAL "GUI")
        set_target_properties(${_target} PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()

    set_target_properties(${_target}
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/$<$<CONFIG:$<CONFIG>>:APP-$<CONFIG>>")

    _iplug_add_target_lib(${_target} IPlug_APP)                  # Add last since it clears the CONFIG_<target>_* variables
    target_link_libraries(${_target} PRIVATE ${_target}-static)  #

    if(NOT IPLUG2_EXTERNAL_PROJECT)
        set_target_properties(${_target} PROPERTIES FOLDER "Examples/${PROJECT_NAME}")
    endif()
endmacro()


#------------------------------------------------------------------------------
# iplug_add_vst3

macro(iplug_add_vst3 _target)
    _iplug_check_initialized()
    set(_oneValueArgs
        "EXTENSION"
        "SUBCATEGORY"
        "CC_UNITNAME"
        "NUM_CC_CHANS"
        "NUM_MIDI_IN_CHANS"
        "NUM_MIDI_OUT_CHANS"
        "PRESET_LIST"
    )

    add_library(${_target} MODULE)

    if(NOT HAVESDK_VST3)
        iplug_warning("VST3 Plugins requires a valid path to Steinberg VST3 SDK. '${_target}' will not be able to compile.")
        set_target_properties(${_target} PROPERTIES
            EXCLUDE_FROM_ALL TRUE
            VS_CONFIGURATION_TYPE Utility) # does an equivalent option for the other generators exist?
    else()
        _iplug_parse_target_arguments(CONFIG_VST3 TARGET_VST3 "" "${_oneValueArgs}" "" "${ARGN}")

        _iplug_add_config_variable(CONFIG_VST3 TARGET_VST3 API 1)

        set(VST3_ICON "${CONFIG_BUNDLE_ICON}")
        if("${VST3_ICON}" STREQUAL "" AND EXISTS "${VST3_SDK_PATH}/doc/artwork/VST_Logo_Steinberg.ico")
            set(VST3_ICON "${VST3_SDK_PATH}/doc/artwork/VST_Logo_Steinberg.ico")
        endif()
        iplug_validate_string(EXTENSION          DEFAULT "vst3" PREFIX CONFIG_VST3 ALPHA NUMERIC HYPHEN UNDERSCORE)
        iplug_validate_string(SUBCATEGORY        DEFAULT "Other" PREFIX CONFIG_VST3 ALPHA DELIMITER MAXLENGTH 127)
        iplug_validate_string(CC_UNITNAME        DEFAULT "MIDI CCs" PREFIX CONFIG_VST3 MAXLENGTH 127)
        iplug_validate_string(NUM_MIDI_IN_CHANS  DEFAULT "1" PREFIX CONFIG_VST3 MINVALUE 1 MAXVALUE 16)
        iplug_validate_string(NUM_MIDI_OUT_CHANS DEFAULT "1" PREFIX CONFIG_VST3 MINVALUE 1 MAXVALUE 16)
        iplug_validate_string(NUM_CC_CHANS       DEFAULT "1" PREFIX CONFIG_VST3 MINVALUE 0 MAXVALUE ${CONFIG_VST3_NUM_MIDI_IN_CHANS})
        iplug_validate_string(PRESET_LIST        DEFAULT "0" PREFIX CONFIG_VST3 STREQUAL 0 1)

        set(PLUGIN_NAME         "${CONFIG_PLUG_NAME}")
        set(PLUGIN_EXT          "${CONFIG_VST3_EXTENSION}")
        set(PLUGIN_NAME_EXT     "${PLUGIN_NAME}.${PLUGIN_EXT}")
        set(VST3_CONFIG_PATH    $<$<CONFIG:$<CONFIG>>:VST3-$<CONFIG>>)
        set(PLUGIN_PACKAGE_PATH "${PROJECT_BINARY_DIR}/bin/${VST3_CONFIG_PATH}/${CONFIG_BUNDLE_NAME}")
        set(_path               "${VST3_SDK_PATH}/public.sdk/source/main")

        set_target_properties(${_target} PROPERTIES
            LIBRARY_OUTPUT_NAME                            "${PLUGIN_NAME}"
            SUFFIX                                         ".${PLUGIN_EXT}"
            LIBRARY_OUTPUT_DIRECTORY                       "${PLUGIN_PACKAGE_PATH}"
            PDB_OUTPUT_DIRECTORY                           "${PROJECT_BINARY_DIR}/PDB/${VST3_CONFIG_PATH}"
            VS_DEBUGGER_COMMAND                            "${IPLUG2_DEFAULT_VST_DEBUG_APPLICATION}"
            BUNDLE                                         TRUE
            BUNDLE_EXTENSION                               "${PLUGIN_EXT}"
            XCODE_ATTRIBUTE_WRAPPER_EXTENSION              "${PLUGIN_EXT}"
            XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE          YES
            XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS $<IF:$<CONFIG:Debug>,YES,NO>
            XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT       $<$<CONFIG:Debug>:dwarf>
        )

        # Implements modified smtg_add_vst3plugin_with_pkgname() / smtg_make_plugin_package()
        # Does *not* include compile with debug information in release mode, nor does it set /DEBUG linker option
        if(MSVC)
            set_target_properties(${_target} PROPERTIES LINK_FLAGS "/DEF:\"${_path}/winexport.def\"")
        elseif(XCODE)
            set_target_properties(${_target} PROPERTIES XCODE_ATTRIBUTE_EXPORTED_SYMBOLS_FILE "${_path}/macexport.exp")
        else()
            set_target_properties(${_target} PROPERTIES LINK_FLAGS "-exported_symbols_list \"${_path}/macexport.exp\"")
        endif()

        # modified smtg_setup_universal_binary()
        if(SMTG_BUILD_UNIVERSAL_BINARY)
            if(XCODE_VERSION VERSION_GREATER_EQUAL 12)
                set_target_properties(${_target} PROPERTIES XCODE_ATTRIBUTE_OSX_ARCHITECTURES "x86_64;arm64;arm64e")
                set_target_properties(${_target} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD_64_BIT)")
            else()
                set_target_properties(${_target} PROPERTIES XCODE_ATTRIBUTE_OSX_ARCHITECTURES "x86_64;i386")
                set_target_properties(${_target} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD_32_64_BIT)")
            endif()
            # diff: adding 'NO' by default unless it's debug build
            set_target_properties(${_target} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<IF:$<CONFIG:Debug>,YES,NO>)
        endif()

        if(SMTG_IOS_DEVELOPMENT_TEAM)
            set_target_properties(${_target} PROPERTIES
                XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${SMTG_IOS_DEVELOPMENT_TEAM}
                XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${SMTG_CODE_SIGN_IDENTITY_MAC}")
        endif()

        if(PLATFORM_WINDOWS)
            if(NOT VST3_ICON STREQUAL "")
                add_custom_command(TARGET ${_target}
                    COMMENT "Copy PlugIn.ico and desktop.ini and change their attributes."
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                        "${VST3_ICON}"
                        "${PLUGIN_PACKAGE_PATH}/PlugIn.ico"
                    COMMAND ${CMAKE_COMMAND} -E copy
                        "${CMAKE_BINARY_DIR}/desktop.ini.in"
                        "${PLUGIN_PACKAGE_PATH}/desktop.ini"
                    COMMAND attrib +h +s "${PLUGIN_PACKAGE_PATH}/desktop.ini"
                    COMMAND attrib +h +s "${PLUGIN_PACKAGE_PATH}/PlugIn.ico"
                    COMMAND attrib +s "${PLUGIN_PACKAGE_PATH}"
                )
            endif()
        elseif(PLATFORM_LINUX)
            # !!! this is untested code
            smtg_get_linux_architecture_name() # Sets var LINUX_ARCHITECTURE_NAME
            set_target_properties(${_target} PROPERTIES PREFIX "")
            if(${CMAKE_BUILD_TYPE} MATCHES "Release|Distributed")
                smtg_strip_symbols(${_target})
            endif()
        endif()

        if(SMTG_RUN_VST_VALIDATOR)
            smtg_run_vst_validator(${_target})
        endif()

        if(SMTG_CREATE_PLUGIN_LINK)
            if(NOT DEFINED VST3_PLUGIN_TARGET_PATH OR VST3_PLUGIN_TARGET_PATH STREQUAL "")
                iplug_syntax_error("Define a proper value for VST3_PLUGIN_TARGET_PATH")
            endif()

            set(TARGET_SOURCE ${PLUGIN_PACKAGE_PATH})
            set(TARGET_DESTINATION ${VST3_PLUGIN_TARGET_PATH}/${CONFIG_BUNDLE_NAME})

            if(PLATFORM_WINDOWS)
                file(TO_NATIVE_PATH "${TARGET_SOURCE}" SOURCE_NATIVE_PATH)
                file(TO_NATIVE_PATH "${TARGET_DESTINATION}" DESTINATION_NATIVE_PATH)
                add_custom_command(
                    TARGET ${_target} POST_BUILD
                    COMMAND if exist "${DESTINATION_NATIVE_PATH}" ( rmdir "${DESTINATION_NATIVE_PATH}" )
                    COMMAND mklink /D
                        "${DESTINATION_NATIVE_PATH}"
                        "${SOURCE_NATIVE_PATH}"
                )
            else()
                add_custom_command(
                    TARGET ${_target} POST_BUILD
                    COMMAND mkdir -p "${TARGET_DESTINATION}"
                    COMMAND ln -svfF "${TARGET_SOURCE}" "${TARGET_DESTINATION}"
                )
            endif()
        endif()

        _iplug_add_target_lib(${_target} IPlug_VST3)                # Add last since it clears the CONFIG_<target>_* variables
        target_link_libraries(${_target} PRIVATE ${_target}-static) #
    endif()

    if(NOT IPLUG2_EXTERNAL_PROJECT)
        set_target_properties(${_target} PROPERTIES FOLDER "Examples/${PROJECT_NAME}")
    endif()
endmacro()

#------------------------------------------------------------------------------
