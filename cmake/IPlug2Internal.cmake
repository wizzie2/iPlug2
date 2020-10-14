include_guard(GLOBAL)

set(_projectConfigArgs
    "IPLUG_EDITOR"
    "IPLUG_DSP"
    "BUNDLE_NAME"
    "BUNDLE_DOMAIN"
    "BUNDLE_ICON"
    "PLUG_NAME"
    "PLUG_NAME_SHORT"
    "PLUG_CLASS_NAME"
    "PLUG_MFR"
    "PLUG_VERSION_STR"
    "PLUG_UNIQUE_ID"
    "PLUG_MFR_ID"
    "PLUG_URL_STR"
    "PLUG_EMAIL_STR"
    "PLUG_COPYRIGHT_STR"
    "PLUG_CHANNEL_IO"
    "PLUG_LATENCY"
    "PLUG_DOES_MIDI_IN"
    "PLUG_DOES_MIDI_OUT"
    "PLUG_DOES_MPE"
    "PLUG_DOES_STATE_CHUNKS"
    "PLUG_HAS_UI"
    "PLUG_WIDTH"
    "PLUG_HEIGHT"
    "PLUG_MIN_WIDTH"
    "PLUG_MIN_HEIGHT"
    "PLUG_MAX_WIDTH"
    "PLUG_MAX_HEIGHT"
    "PLUG_FPS"
    "PLUG_SHARED_RESOURCES"
    "PLUG_TYPE"
    "PLUG_HOST_RESIZE"
    "PLUG_TFLOAT_TYPE"
    "SHARED_RESOURCES_SUBPATH"
    "PCH_FOLDER_NAME"
    "OBJC_PREFIX"
)

set(_projectConfigArgs_overridable ${_projectConfigArgs})
list(REMOVE_ITEM _projectConfigArgs_overridable
    "PLUG_MFR"
    "PLUG_VERSION_STR"
    "PCH_FOLDER_NAME"
)

# configuration variables that are considered string values and gets quotation marks applied in the definition
set(_iplug_config_string_variables
    PLUG_NAME
    PLUG_NAME_ABBREVIATION
    PLUG_MFR
    PLUG_VERSION_STR
    PLUG_URL_STR
    PLUG_EMAIL_STR
    PLUG_COPYRIGHT_STR
    PLUG_CHANNEL_IO
    BUNDLE_NAME
    BUNDLE_MFR
    BUNDLE_DOMAIN
    SHARED_RESOURCES_SUBPATH
    VST3_SUBCATEGORY
    VST3_CC_UNITNAME
    AAX_PLUG_MFR_STR
    AAX_PLUG_NAME_STR
    AAX_PLUG_CATEGORY_STR
    AUV2_ENTRY_STR
    AUV2_VIEW_CLASS_STR
)

# list of config variables excluded from being defined in code
set(_iplug_config_definition_exclude
    PCH_FOLDER_NAME
    INCLUDE_DIRECTORIES
    SOURCES
    LINK_LIBRARIES
    BUNDLE_ICON
    APP_SUBSYSTEM
    VST3_EXTENSION
)


#------------------------------------------------------------------------------
# _iplug_pre_project_setup
#
#   Pre Project initialization of everything related to IPlug2
macro(_iplug_pre_project_setup)

    validate_path("${IPLUG2_ROOT_PATH}" "IPLUG2_ROOT_PATH path not set correctly")

    # Standard in-source build prevention
    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
        iplug_syntax_error(
            "Generating build files in source tree is not allowed to prevent sleepless nights and lots of tears."
            "Create a build directory outside of the source code and call cmake from there.")
    endif()

    #------------------------------------------------------------------------------
    # Get IPlug2 verion number from ROOT/IPlug/IPlugVersion.h
    iplug_get_version("${IPLUG2_ROOT_PATH}/IPlug/IPlugVersion.h" IPLUG_VERSION)
    iplug_info("IPlug2 version ${IPLUG_VERSION}")

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
endmacro()


#------------------------------------------------------------------------------
# _iplug_post_project_setup
#
#   Post Project initialization of everything related to IPlug2

macro(_iplug_post_project_setup)
    string_assert(${CMAKE_SYSTEM_NAME} "CMAKE_SYSTEM_NAME not set in toolchain")

    set(PLATFORM_Windows    "PLATFORM_WINDOWS" "Windows")
    set(PLATFORM_Darwin     "PLATFORM_MAC"     "Mac"    )
    set(PLATFORM_Linux      "PLATFORM_LINUX"   "Linux"  )
    set(PLATFORM_iOS        "PLATFORM_IOS"     "IOS"    )
#    set(PLATFORM_Android    "PLATFORM_ANDROID" "Android")

    list(GET PLATFORM_${CMAKE_SYSTEM_NAME} 0 PLATFORM_ID)
    list(GET PLATFORM_${CMAKE_SYSTEM_NAME} 1 PLATFORM_NAME)

    string_assert(${PLATFORM_ID}   "PLATFORM_ID \"${CMAKE_SYSTEM_NAME}\" is not supported yet.")
    string_assert(${PLATFORM_NAME} "PLATFORM_NAME missing from PLATFORM_${CMAKE_SYSTEM_NAME}"  )

    set(${PLATFORM_ID} TRUE)
    set(PLATFORM_APPLE ${APPLE})  # set if target is macOS, iOS, tvOS or watchOS

    # Set build types
    set(CONFIGURATION_TYPES "Debug" "Release" "Distributed")
    set(DEFAULT_BUILD_TYPE "Debug")

    get_property(bIsMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(bIsMultiConfig)
        set(CMAKE_CONFIGURATION_TYPES ${CONFIGURATION_TYPES} CACHE STRING "" FORCE)
        mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
    else()
        if(NOT CMAKE_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "" FORCE)
            set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
            set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CONFIGURATION_TYPES})
        endif()
    endif()

    # Configure output folders
    if(PLATFORM_WINDOWS OR (PLATFORM_MAC AND CMAKE_GENERATOR STREQUAL Xcode))
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    else ()
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
    endif()

    find_package(Git)

    if(PLATFORM_WINDOWS AND NOT EXISTS "${CMAKE_BINARY_DIR}/desktop.ini.in")
        file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/desktop.ini.in"
            CONTENT "[.ShellClassInfo]\nIconResource=PlugIn.ico,0\n")
    endif()

    #------------------------------------------------------------------------------
    # VST3 SDK

    set(_path $ENV{VST3_SDK_PATH})
    if(NOT _path)
        set(_path "${IPLUG2_ROOT_PATH}/Dependencies/IPlug/VST3_SDK")
    endif()
    set(VST3_SDK_PATH "${_path}" CACHE PATH "Path to Steinberg VST3 SDK")

    set(_files
        "CMakeLists.txt"
        "base/CMakeLists.txt"
        "pluginterfaces/CMakeLists.txt"
        "public.sdk/CMakeLists.txt"
        "public.sdk/source/main/winexport.def"
        "public.sdk/source/main/macexport.exp"
    )
    set(HAVESDK_VST3 TRUE)
    foreach(_file IN LISTS _files)
        if(NOT EXISTS "${VST3_SDK_PATH}/${_file}")
            set(HAVESDK_VST3 FALSE)
        endif()
    endforeach()

    if(HAVESDK_VST3 OR HAVESDK_VST2)
        # Find a suitable application to use as default when launching debugger for VST3 plugins
        if(CMAKE_GENERATOR MATCHES "^Visual Studio")
            set(_reg_tests
                "[HKEY_LOCAL_MACHINE\\SOFTWARE\\REAPER]"
                "[HKEY_LOCAL_MACHINE\\SOFTWARE\\PreSonus\\Studio One 5]"
                "[HKEY_LOCAL_MACHINE\\SOFTWARE\\PreSonus\\Studio One 4]"
                "[HKEY_LOCAL_MACHINE\\SOFTWARE\\PreSonus\\Studio One 3]"
                # "[HKEY_CURRENT_USER\\Software\\Image-Line\\FL Studio 20\\General;Program location]"  # unconfirmed
                # "[HKEY_CURRENT_USER\\Software\\Image-Line\\FL Studio 12\\General;Program location]"  # unconfirmed
                # "[HKEY_LOCAL_MACHINE\\Software\\Propellerhead Software\\Reason]"                     # unconfirmed
            )

            set(IPLUG2_DEFAULT_VST_DEBUG_APPLICATION_DESC "Default application to use when debugging a VST plugins. This setting does not override any previous settings in Visual Studio.")
            set(IPLUG2_DEFAULT_VST_DEBUG_APPLICATION "" CACHE FILEPATH "${IPLUG2_DEFAULT_VST_DEBUG_APPLICATION_DESC}")
            if(NOT DEFINED IPLUG2_DEFAULT_VST_DEBUG_APPLICATION OR IPLUG2_DEFAULT_VST_DEBUG_APPLICATION STREQUAL "")
                foreach(_key IN LISTS _reg_tests)
                    get_filename_component(_key "${_key}" ABSOLUTE)
                    if(_key AND NOT _key MATCHES "registry")
                        if(_key MATCHES "^.*Reaper$")
                            string(APPEND _key "/reaper.exe" )
                        endif()
                        if(EXISTS ${_key})
                            set(IPLUG2_DEFAULT_VST_DEBUG_APPLICATION ${_key} CACHE FILEPATH "${IPLUG2_DEFAULT_VST_DEBUG_APPLICATION_DESC}" FORCE)
                            break()
                        endif()
                    endif()
                endforeach()
            endif()
        endif()
    endif()

    if(HAVESDK_VST3)
        list(APPEND CMAKE_MODULE_PATH "${VST3_SDK_PATH}/cmake/modules")

        include(SMTG_DetectPlatform)
        smtg_detect_platform()

        include(SetupVST3LibraryDefaultPath)
        include(AddVST3Library)
        include(CoreAudioSupport)

        smtg_get_default_vst3_path()  # Provides DEFAULT_VST3_FOLDER

        set(VST3_PLUGIN_TARGET_PATH "${DEFAULT_VST3_FOLDER}" CACHE PATH "Here you can redefine the VST3 Plug-ins folder")

        # Unlike the default setting in Steinberg VST3 SDK, we do *not* create a directory if it's missing, just warn about it.
        if(VST3_PLUGIN_TARGET_PATH AND NOT EXISTS ${VST3_PLUGIN_TARGET_PATH})
            iplug_warning("VST3_PLUGIN_TARGET_PATH = \"${VST3_PLUGIN_TARGET_PATH}\" is invalid.")
        endif()

        if(PLATFORM_WINDOWS)
            set(DEF_OPT_LINK OFF) # be sure to start visual with admin right when enabling this
        else()
            set(DEF_OPT_LINK ON)
        endif()
        option(SMTG_CREATE_PLUGIN_LINK "Create symbolic link for each Plug-in (you need to have the Administrator right on Windows! or change the Local Group Policy to allow create symbolic links)" ${DEF_OPT_LINK})

        if(PLATFORM_APPLE)
            set(SMTG_CODE_SIGN_IDENTITY_MAC "Mac Developer" CACHE STRING "macOS Code Sign Identity")
            set(SMTG_CODE_SIGN_IDENTITY_IOS "iPhone Developer" CACHE STRING "iOS Code Sign Identity")
            option(SMTG_AUV3_WRAPPER_EXTERNAL_PLUGIN_FACTORY "Use externally provided GetPluginFactory()" OFF)
        endif()

        # Run the Validator after each new compilation of VST3 Plug-ins
        option(SMTG_RUN_VST_VALIDATOR "Run VST validator on VST3 Plug-ins" ON)
    endif()

    #------------------------------------------------------------------------------
    # VST2 SDK

    # if(PLATFORM_WINDOWS)
    #     get_filename_component(DEFAULT_VST2_FOLDER "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VST;VSTPluginsPath]" ABSOLUTE)
    #     cmake_print_variables(DEFAULT_VST2_FOLDER)
    # endif()


    #------------------------------------------------------------------------------
    # Glad version settings

    set(IPLUG2_GLAD_VERSION "4.5-ES2-3.1-Core" CACHE STRING "Select version of Glad to compile for OpenGL")
    set_property(CACHE IPLUG2_GLAD_VERSION PROPERTY STRINGS
        "2.1-Compability"
        "2.1-Core"
        "3.3-Compability"
        "3.3-Core"
        "4.5-ES2-3.1-Compability"
        "4.5-ES2-3.1-Core"
    )


    #------------------------------------------------------------------------------
    # To keep options within a somewhat reasonable level, we choose graphics backend in cmake options,
    # not per target. This way we can bake it into the static library. Though, every target will
    # have to use the same graphics backend. Unfortunately this also means that we can't compile
    # for multiple plugins if we're targeting a web application (at the moment).

    # TODO: Add support for the other graphics libraries

    set(IPLUG2_GFXLIBRARY "NanoVG" CACHE STRING "Select backend graphics library for rendering")
    set_property(CACHE IPLUG2_GFXLIBRARY PROPERTY STRINGS
        None
        # AGG
        # Cairo
        # Lice
        NanoVG
        # Skia
        # Canvas
    )
    string(TOUPPER ${IPLUG2_GFXLIBRARY} IPLUG2_GFXLIBRARY)
    unset(GFXLIB_NONE)
    unset(GFXLIB_AGG)
    unset(GFXLIB_CAIRO)
    unset(GFXLIB_LICE)
    unset(GFXLIB_NANOVG)
    unset(GFXLIB_SKIA)
    unset(GFXLIB_CANVAS)
    set(GFXLIB_${IPLUG2_GFXLIBRARY} TRUE)
    if(PLATFORM_APPLE)
        option(IPLUG2_ENABLE_METAL "Use Metal for rendering" ON)
    endif()
endmacro()

#------------------------------------------------------------------------------
macro(_iplug_check_initialized)
    string_assert(${CMAKE_SYSTEM_NAME} "CMAKE_SYSTEM_NAME not set in toolchain.")
    if(NOT IPLUG_IS_INITIALIZED)
        _iplug_post_project_setup()
        _iplug_set_default_compiler_options()
        set(IPLUG_IS_INITIALIZED TRUE)
    endif()
endmacro()

#------------------------------------------------------------------------------
macro(_iplug_set_ifndef _var _value)
    if(NOT DEFINED ${_var})
        set(${_var} ${_value})
    endif()
endmacro()

#------------------------------------------------------------------------------
macro(_iplug_set_default_compiler_options)

    # Required compiler version checking
    if(MSVC AND NOT Clang)
        # The minimum compatible compiler version still needs adjusting
        if(MSVC_VERSION LESS 1911)
            iplug_syntax_error("MSVC++ version 14.11 or higher is required.")
        elseif(MSVC_TOOLSET_VERSION LESS 141)
            iplug_syntax_error("MSVC Toolset v141 or higher is required.")
        endif()
    elseif(GNU OR Clang OR AppleClang)
        # TODO: add minimum version check for these compilers
    endif()

    # Option to set C++ standard without affecting any files under source control.
    set(IPLUG2_CXX_STANDARD "Default" CACHE STRING "C++ language standard to use. Default option uses project settings if specified, otherwise C++17.")
    set_property(CACHE IPLUG2_CXX_STANDARD PROPERTY STRINGS Default C++17 C++20)
    if(IPLUG2_CXX_STANDARD STREQUAL "Default")
        _iplug_set_ifndef(CMAKE_CXX_STANDARD 17)
    elseif(IPLUG2_CXX_STANDARD STREQUAL "C++17")
        set(CMAKE_CXX_STANDARD 17)
    elseif(IPLUG2_CXX_STANDARD STREQUAL "C++20")
        set(CMAKE_CXX_STANDARD 20)
    endif()

    _iplug_set_ifndef(CMAKE_CXX_STANDARD_REQUIRED YES )
    _iplug_set_ifndef(CMAKE_CXX_EXTENSIONS        OFF )

    # Default symbols visibility
    _iplug_set_ifndef(CMAKE_CXX_VISIBILITY_PRESET         hidden )
    _iplug_set_ifndef(CMAKE_CXX_VISIBILITY_INLINES_HIDDEN YES    )

    option(IPLUG2_USE_TUNED_COMPILER_SETTINGS "Use optimized settings for the compiler and linker." ON)

    if(IPLUG2_USE_TUNED_COMPILER_SETTINGS)
        if(MSVC AND NOT Clang)
            set(_obx /Ob3)
            # Ob3 requires VC++ 16.3 (MSVC_VERSION 1923) or higher
            if(MSVC_VERSION LESS 1923)
                set(_obx /Ob2)
            endif()

            # Flags that are set as default for every new target configuration
            set(CL_FLAGS
                /D_CRT_SECURE_NO_DEPRECATE    # Disable deprecation warnings for Unsafe CRT Library functions
                /D_CRT_NONSTDC_NO_DEPRECATE   # Disable deprecation warnings for POSIX function names
                /D_MBCS                       # _UNICODE, _MBCS or _SBCS. _MBCS is the same as _SBCS but with additional typesafety checks
                                              # If using _UNICODE, it's recommended to set both _UNICODE and UNICODE.

                /GR               # Enable Run-Time Type Information
                /EHsc             # Exception handling model, assume extern "C" functions never throw a C++ exception
                /W3               # Warning level 3 (Default)
                /MP               # Build with Multiple Processes
                /arch:AVX         # Enables the use of Intel Advanced Vector Extensions instructions
                /fp:fast          # Floating Point Model '/fp:fast'. This is also the default used by Steinberg VST3 SDK.
                /Zc:preprocessor  # C++ conforming preprocessor
                /Zc:rvalueCast    # Enforce type conversion rules. Conform to the C++11 standard
                /Zc:__cplusplus   # Enables the __cplusplus preprocessor macro to report an updated value for recent C++ language standards support. If not specified __cplusplus will always be 199711L
                /volatile:iso     # Strict volatile semantics. Acquire/release semantics are not guaranteed
                /utf-8            # Specifies UTF-8 character set, this is the default for GCC & Clang
                /Zp8              # Packs structures on 8-byte boundaries. Default is 16
                /TP               # Treat all as C++ source files
                /permissive-      # Set standard-conformance mode. "Should" be default since VC++ 2017 v15.5
                /GF               # Eliminate Duplicate Strings (string pooling) (Enabled in debug as well to avoid behaviour differences)
                /JMC              # Just My Code debugging
                /wd5045           # Disable Spectre mitigation warnings
                /wd4652           # Compiler option 'option' inconsistent with precompiled header; current command-line option will override that defined in the precompiled header
            )

            # Debug
            set(CL_FLAGS_DEBUG
                /D_DEBUG          # Specify debug versions of the C run-time library
                /GS               # Buffer Security Checks. Detects some buffer overruns that overwrite a function's return address, exception handler address, or certain types of parameters
                /Zi               # Generates complete debugging information. /ZI (edit and continue) can cause issues in code size, performance, and compiler conformance
                /Ob0              # Disables inline expansions
                /Od               # Disables optimization
                /RTC1             # Stack frame and uninitialized variable run-time error checking
            )

            # Release
            set(CL_FLAGS_RELEASE
                /DNDEBUG          # Turn off assertion checks
                /GS               # Buffer Security Checks. Detects some buffer overruns that overwrite a function's return address, exception handler address, or certain types of parameters
                /Zo               # Enhance Optimized Debugging for optimized code in non-debug builds. Tells the compiler to generate additional debugging information for local variables and inlined functions
                /GL               # Whole Program Optimization
                /O2               # Creates fast code
                /Oi               # Generates intrinsic functions
                /Ob2              # Inline Function Expansion
                /Gy               # Enables function-level linking
                /Oy               # Suppresses creation of frame pointers on the call stack (x86 only)
                /Ot               # Favor Fast Code (implied by the /O2, but just to be sure)
            )

            # Distributed (lean and mean version, ready for worldwide distribution)
            set(CL_FLAGS_DISTRIBUTED
                /DNDEBUG          # Turn off assertion checks
                /DDISTRIBUTED=1   # Set DISTRIBUTED preprocessor variable
                /GL               # Whole Program Optimization
                /Oi               # Generates intrinsic functions
                /O2               # Creates fast code
                ${_obx}           # Inline Function Expansion (/Ob3 if supported, otherwise /Ob2)
                /Gy               # Enables function-level linking
                /GS-              # Disable buffers security checks
                /Oy               # Suppresses creation of frame pointers on the call stack (x86 only)
                /Ot               # Favor Fast Code (implied by the /O2, but just to be sure)
            )

            list(JOIN CL_FLAGS             " " CL_FLAGS            )
            list(JOIN CL_FLAGS_DEBUG       " " CL_FLAGS_DEBUG      )
            list(JOIN CL_FLAGS_RELEASE     " " CL_FLAGS_RELEASE    )
            list(JOIN CL_FLAGS_DISTRIBUTED " " CL_FLAGS_DISTRIBUTED)

            foreach(_lang IN ITEMS C CXX)
                set(CMAKE_${_lang}_FLAGS             ${CL_FLAGS}            )
                set(CMAKE_${_lang}_FLAGS_DEBUG       ${CL_FLAGS_DEBUG}      )
                set(CMAKE_${_lang}_FLAGS_RELEASE     ${CL_FLAGS_RELEASE}    )
                set(CMAKE_${_lang}_FLAGS_DISTRIBUTED ${CL_FLAGS_DISTRIBUTED})
            endforeach()

            # Use multithreaded, static versions of the MSVC run-time library (LIBCMT.lib/LIBCMTD.lib)
            _iplug_set_ifndef(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

            foreach(_type IN ITEMS EXE SHARED MODULE)
                foreach(_cfg RELEASE DISTRIBUTED)
                    set(CMAKE_${_type}_LINKER_FLAGS_${_cfg} "/LTCG /INCREMENTAL:NO")
                endforeach()
            endforeach()

        elseif(GNU OR Clang OR AppleClang)

            set(CL_FLAGS
                -fmessage-length=0
                -pipe
                -fexceptions
                -fasm-blocks
                -fno-math-errno
                -fno-trapping-math
                -mavx
                -Wdelete-non-virtual-dtor
            )

            set(CL_FLAGS_DEBUG
                /D_DEBUG
                $<$<BOOL:${enable_address_sanitizer}>:-fsanitize=address>
                $<$<BOOL:${enable_thread_sanitizer}>:-fsanitize=thread>
                $<$<BOOL:${enable_undefined_behavior_sanitizer}>:-fsanitize=undefined>
            )

            set(CL_FLAGS_RELEASE
                /DNDEBUG
                -O3
            )

            set(CL_FLAGS_DISTRIBUTED
                /DNDEBUG
                /DDISTRIBUTED=1
                -O3
            )

            list(JOIN CL_FLAGS             " " CL_FLAGS            )
            list(JOIN CL_FLAGS_DEBUG       " " CL_FLAGS_DEBUG      )
            list(JOIN CL_FLAGS_RELEASE     " " CL_FLAGS_RELEASE    )
            list(JOIN CL_FLAGS_DISTRIBUTED " " CL_FLAGS_DISTRIBUTED)

            # Appending settings since I don't know what cmake defaults are for these compilers.
            foreach(_lang IN ITEMS C CXX)
                string(APPEND CMAKE_${_lang}_FLAGS             " ${CL_FLAGS}"            )
                string(APPEND CMAKE_${_lang}_FLAGS_DEBUG       " ${CL_FLAGS_DEBUG}"      )
                string(APPEND CMAKE_${_lang}_FLAGS_RELEASE     " ${CL_FLAGS_RELEASE}"    )
                string(APPEND CMAKE_${_lang}_FLAGS_DISTRIBUTED " ${CL_FLAGS_DISTRIBUTED}")
                # set(CMAKE_${_lang}_FLAGS             ${CL_FLAGS}            )
                # set(CMAKE_${_lang}_FLAGS_DEBUG       ${CL_FLAGS_DEBUG}      )
                # set(CMAKE_${_lang}_FLAGS_RELEASE     ${CL_FLAGS_RELEASE}    )
                # set(CMAKE_${_lang}_FLAGS_DISTRIBUTED ${CL_FLAGS_DISTRIBUTED})
            endforeach()

            foreach(_type IN ITEMS EXE SHARED MODULE)
                foreach(_cfg RELEASE DISTRIBUTED)
                    string(APPEND CMAKE_${_type}_LINKER_FLAGS_${_cfg} " $<$<CXX_COMPILER_ID:AppleClang>:-dead_strip>")
                    # set(CMAKE_${_type}_LINKER_FLAGS_${_cfg} "$<$<CXX_COMPILER_ID:AppleClang>:-dead_strip>")
                endforeach()
            endforeach()
        endif()
    else()
        foreach(_lang IN ITEMS C CXX)
            _iplug_set_ifndef(CMAKE_${_lang}_FLAGS_DISTRIBUTED "${CMAKE_${_lang}_FLAGS_RELEASE} /DDISTRIBUTED=1")
        endforeach()

        foreach(_type IN ITEMS EXE SHARED MODULE)
            _iplug_set_ifndef(CMAKE_${_type}_LINKER_FLAGS_DISTRIBUTED "${CMAKE_${_type}_LINKER_FLAGS_RELEASE}")
        endforeach()
    endif()
endmacro()


#------------------------------------------------------------------------------
# _iplug_disable_source_compile

function(_iplug_disable_source_compile)
    cmake_parse_arguments(_arg "" "REGEX" "" ${ARGN})
    set(_exclude "")
    if(_arg_REGEX)
        set(SOURCE_LIST ${_arg_UNPARSED_ARGUMENTS})
        foreach(_element ${SOURCE_LIST})
            string(REGEX MATCH "${_arg_REGEX}" _result ${_element})
            if(_result)
                list(APPEND _exclude ${_element})
                iplug_debug_message("${_element}")
            endif()
        endforeach()
    else()
        set(_exclude ${_arg_UNPARSED_ARGUMENTS})
    endif()
    set_source_files_properties(${_exclude} PROPERTIES LANGUAGE "")
endfunction()


#------------------------------------------------------------------------------
# _iplug_parse_target_arguments

macro(_iplug_parse_target_arguments _config_prefix _definition_prefix _optionsArgs _oneValueArgs _multiValueArgs)
    set(_multiValue ${_multiValueArgs} "OVERRIDE")
    cmake_parse_arguments(_arg "${_optionsArgs}" "${_oneValueArgs}" "${_multiValue}" ${ARGN})
    _iplug_warn_unparsed_arguments("" _arg_UNPARSED_ARGUMENTS)
    foreach(_var IN LISTS _oneValueArgs)
        _iplug_add_config_variable(${_config_prefix} ${_definition_prefix} ${_var} "${_arg_${_var}}")
    endforeach()
    cmake_parse_arguments(_override  "" "${_projectConfigArgs_overridable}" "" ${_arg_OVERRIDE})
    _iplug_warn_unparsed_arguments("[OVERRIDE]" _override_UNPARSED_ARGUMENTS)
    foreach(_var IN LISTS _projectConfigArgs_overridable)
        if(DEFINED _override_${_var})
            _iplug_add_config_variable(CONFIG_OVERRIDE "" ${_var} "${_override_${_var}}")
        endif()
    endforeach()
    _iplug_validate_config_variables(CONFIG_OVERRIDE DEFINED)
endmacro()

#------------------------------------------------------------------------------
# _iplug_warn_unparsed_arguments

function(_iplug_warn_unparsed_arguments _pre_str _list)
    if(DEFINED _pre_str)
        string(APPEND _pre_str " ")
    endif()
    set(_list ${${_list}})
    if(_list)
        while(TRUE)
            list(LENGTH _list _len)
            if(NOT _len OR _len EQUAL 0)
                break()
            endif()
            list(POP_FRONT _list _item_name)
            list(POP_FRONT _list _item_val)
            iplug_warning("Unknown configuration ${_pre_str}${_item_name} = \"${_item_val}\".")
        endwhile()
    endif()
endfunction()

#------------------------------------------------------------------------------
# _iplug_add_config_variable

function(_iplug_add_config_variable _config_prefix _definition_prefix _name _value)
    if(NOT _config_prefix STREQUAL "")
        string(APPEND _config_prefix "_")
    endif()
    if(NOT _definition_prefix STREQUAL "")
        string(APPEND _definition_prefix "_")
    endif()

    string(REPLACE "\n" "\\n" _str "${_value}")
    string(REPLACE "\r" "\\r" _str "${_str}")
    string(REPLACE "\t" "\\t" _str "${_str}")

    set(${_config_prefix}${_name} "${_str}" PARENT_SCOPE)
    if(NOT "${_name}" MATCHES "^OVERRIDE_.*")
        set(CONFIG_VARIABLES ${CONFIG_VARIABLES} "${_definition_prefix}${_name}" PARENT_SCOPE)
    endif()
endfunction()


#------------------------------------------------------------------------------
# _iplug_validate_config_variables

macro(_iplug_validate_config_variables _prefix)
    cmake_parse_arguments(_arg  "" "" "" ${ARGN})
    set(_extraFlags ${_arg_UNPARSED_ARGUMENTS})

    set(VALIDATION_PLUG_TFLOAT_TYPE         DEFAULT "float" NOTEMPTY STREQUAL float double)
    set(VALIDATION_PLUG_NAME                DEFAULT "${PROJECT_NAME}" NOTEMPTY ALPHA NUMERIC SPACE)
    set(VALIDATION_PLUG_NAME_SHORT          NOTEMPTY ALPHA NUMERIC MINLENGTH 4 MAXLENGTH 4)
    set(VALIDATION_PLUG_CLASS_NAME          DEFAULT "${PROJECT_NAME}" NOTEMPTY ALPHAFIRST ALPHA NUMERIC UNDERSCORE)
    set(VALIDATION_PLUG_MFR                 DEFAULT "AcmeInc" NOTEMPTY MAXLENGTH 127)
    set(VALIDATION_PLUG_VERSION_STR         DEFAULT "1.0.0" NOTEMPTY NUMERIC VERSION 3)
    set(VALIDATION_PLUG_URL_STR             MAXLENGTH 255)
    set(VALIDATION_PLUG_EMAIL_STR           MAXLENGTH 127)
    set(VALIDATION_PLUG_COPYRIGHT_STR       MAXLENGTH 127)
    set(VALIDATION_PLUG_UNIQUE_ID           NOTEMPTY SINGLE_QUOTED ALPHA NUMERIC MAXLENGTH 4)
    set(VALIDATION_PLUG_MFR_ID              DEFAULT "'Acme'" NOTEMPTY SINGLE_QUOTED ALPHA NUMERIC MAXLENGTH 4)
    set(VALIDATION_PLUG_CHANNEL_IO          DEFAULT "0-2" NOTEMPTY NUMERIC HYPHEN DOT SPACE)
    set(VALIDATION_PLUG_LATENCY             DEFAULT "0" MINVALUE 0)
    set(VALIDATION_PLUG_DOES_MIDI_IN        DEFAULT "0" STREQUAL 0 1)
    set(VALIDATION_PLUG_DOES_MIDI_OUT       DEFAULT "0" STREQUAL 0 1)
    set(VALIDATION_PLUG_DOES_MPE            DEFAULT "0" STREQUAL 0 1)
    set(VALIDATION_PLUG_DOES_STATE_CHUNKS   DEFAULT "0" STREQUAL 0 1)
    set(VALIDATION_PLUG_HAS_UI              DEFAULT "1" STREQUAL 0 1)
    set(VALIDATION_PLUG_WIDTH               DEFAULT "1024" MINVALUE 100)
    set(VALIDATION_PLUG_HEIGHT              DEFAULT "768" MINVALUE 100)
    set(VALIDATION_PLUG_MIN_WIDTH           DEFAULT "256" MINVALUE 100)
    set(VALIDATION_PLUG_MIN_HEIGHT          DEFAULT "256" MINVALUE 100)
    set(VALIDATION_PLUG_HOST_RESIZE         DEFAULT "0" NOTEMPTY STREQUAL 0 1)
    set(VALIDATION_PLUG_FPS                 DEFAULT "60" MINVALUE 10 MAXVALUE 1000)
    set(VALIDATION_PLUG_SHARED_RESOURCES    DEFAULT "0" STREQUAL 0 1)
    set(VALIDATION_PLUG_TYPE                DEFAULT "Effect" NOTEMPTY STREQUAL Effect Instrument MIDIEffect)
    set(VALIDATION_BUNDLE_ICON              DEFAULT "${DEFAULT_ICON}" FILE_EXISTS ABSOLUTE)
    set(VALIDATION_BUNDLE_DOMAIN            DEFAULT "com" NOTEMPTY ALPHAFIRST ALPHA NUMERIC HYPHEN)
    set(VALIDATION_BUNDLE_NAME              DEFAULT "${PROJECT_NAME}" NOTEMPTY ALPHAFIRST ALPHA NUMERIC HYPHEN)
    set(VALIDATION_SHARED_RESOURCES_SUBPATH DEFAULT "${PROJECT_NAME}")
    set(VALIDATION_OBJC_PREFIX              ALPHA NUMERIC UNDERSCORE MINLENGTH 3 MAXLENGTH 31)
    set(VALIDATION_IPLUG_EDITOR             DEFAULT "1"  STREQUAL 0 1)
    set(VALIDATION_IPLUG_DSP                DEFAULT "1"  STREQUAL 0 1)

    foreach(_cfg IN LISTS _projectConfigArgs)
        iplug_validate_string(${_cfg} PREFIX ${_prefix} ${_extraFlags} ${VALIDATION_${_cfg}})
    endforeach()
endmacro()


#------------------------------------------------------------------------------
# _iplug_generate_source_groups

function(_iplug_generate_source_groups)
    # Set up source groups for IDE

    # Move precompiled headers into their own IDE group
    if("${CONFIG_PCH_FOLDER_NAME}" STREQUAL "")
        set(CONFIG_PCH_FOLDER_NAME "resources/Precompiled Headers")
    endif()
    source_group("${CONFIG_PCH_FOLDER_NAME}" REGULAR_EXPRESSION "^.*CMakeFiles.*cmake_pch\\..*xx$")

    get_target_property(_src_list IPlug INTERFACE_SOURCES)
    source_group(TREE "${IPLUG2_ROOT_PATH}" FILES ${_src_list})

    get_target_property(_src_list IGraphics INTERFACE_SOURCES)
    source_group(TREE "${IPLUG2_ROOT_PATH}" FILES ${_src_list})


    # Platform filter source files
    get_target_property(_iplug_src_list IPlug INTERFACE_SOURCES)
    get_target_property(_igraphics_src_list IGraphics INTERFACE_SOURCES)
    set(_src_list ${_iplug_src_list} ${_igraphics_src_list} )

    # Want to buy: better naming consistency, platform should be
    # prefix or suffix, not in the middle of the file name :)
    if(NOT PLATFORM_WINDOWS)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*Windows.*\\.cpp$")
        _iplug_disable_source_compile(${_src_list} REGEX "^.*Win.*\\.cpp$")
    endif()

    if(NOT PLATFORM_MAC)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*Mac.*\\.(m|mm|cpp)$")
    endif()

    if(NOT PLATFORM_IOS)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*IOS.*\\.(m|mm|cpp)$")
    endif()

    if(NOT PLATFORM_LINUX)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*Linux.*\\.cpp$")
    endif()

    if(NOT PLATFORM_APPLE)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*\\.(m|mm)$")
    endif()

    # Disable if associated SDK isn't available
    if(NOT HAVESDK_AAX)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*AAX.*\\.cpp$")
    endif()

    if(NOT HAVESDK_AU)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*AUv2.*\\.(m|mm|c|cpp)$")
    endif()

    if(NOT HAVESDK_AUv3)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*AUv3.*\\.(m|mm|cpp)$")
    endif()

    if(NOT HAVESDK_VST2)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*VST2.*\\.cpp$")
    endif()

    if(NOT HAVESDK_VST3)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*VST3.*\\.cpp$")
    endif()

    # TODO: temporary, separate Web to target instead of platform
    if(NOT PLATFORM_WEB)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*Web.*\\.cpp$")
        _iplug_disable_source_compile(${_src_list} REGEX "^.*WEB.*\\.cpp$")
    endif()

    if(GFXLIB_NONE)
        _iplug_disable_source_compile(${_src_list} REGEX "^${IPLUG2_ROOT_PATH}/IGraphics/.*\\.cpp$")
    endif()

    # Disable code for unused graphics libraries
    if(NOT GFXLIB_CANVAS)
        _iplug_disable_source_compile(
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsCanvas.cpp")
    endif()

    if(NOT GFXLIB_AGG)
        _iplug_disable_source_compile(
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsAGG.cpp"
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsAGG_src.cpp"
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsAGG_src.mm")
    endif()

    if(NOT GFXLIB_CAIRO)
        _iplug_disable_source_compile(
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsCairo.cpp")
    endif()

    if(NOT GFXLIB_LICE)
        _iplug_disable_source_compile(
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsLice.cpp"
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsLice_src.cpp"
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsLice_src.mm")
    endif()

    if(NOT GFXLIB_NANOVG)
        _iplug_disable_source_compile(
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsNanoVG.cpp"
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsNanoVG_src.m")
    endif()

    if(NOT GFXLIB_SKIA)
        _iplug_disable_source_compile(
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsSkia.cpp"
            "${IPLUG2_ROOT_PATH}/IGraphics/Drawing/IGraphicsSkia_src.cpp")
    endif()

    # Option to compile with Faust if available
    # option(IPLUG2_EXTRAS_FAUST "Compile with Faust." OFF)
    # if(NOT IPLUG2_EXTRAS_FAUST)
        _iplug_disable_source_compile(${_src_list} REGEX "^.*Faust.*\\.cpp$")
    # endif()

    # Option to compile with reaper extension API
    # option(IPLUG2_EXTRAS_REAPEREXT "Compile with Reaper Extension API." OFF)
    # if(NOT IPLUG2_EXTRAS_REAPEREXT)
        _iplug_disable_source_compile("${IPLUG2_ROOT_PATH}/IPlug/ReaperExt/ReaperExtBase.cpp")
    # endif()

endfunction()

#------------------------------------------------------------------------------
# _iplug_add_target_lib
#
# This will create a static library related to the current consumer.
# Since IPlug2 have a lot of conditional compiling in #if statements and
# rely on config.h from the consumer for information, we make a target
# specific static library to prevent recompilation of all the code
# in the consumer project.
#
#   _target         - Name of the target to build a static library for
#   _pluginapi_lib  - Target type library to link

function(_iplug_add_target_lib _target _pluginapi_lib)

    set(_libName "${_target}-static")
    add_library(${_libName})

    # Add access to target specific resources
    target_include_directories(${_libName}
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/resources
    )

    set_target_properties(${_target} PROPERTIES
        VS_DPI_AWARE "PerMonitor"
    )

    # Get list of all source files in IPlug and IGraphics.
    # We can't use the interface libraries directly since we have to filter
    # out some of the source files depending on what the target type is.
    get_target_property(_iplug_src_list IPlug INTERFACE_SOURCES)
    get_target_property(_igraphics_src_list IGraphics INTERFACE_SOURCES)
    set(_src_list ${_iplug_src_list} ${_igraphics_src_list})

    # Filter out unused sources from compiling.
    # Don't like this solution, but since you can only set flags per 'source file'
    # instead of per 'target/source file', this will have to do.
    # Preferably we would like to see all source files listed in the IDE and just
    # exclude those we don't need from compiling on a per target instead. But, since
    # we can have multiple targets for the same source file per project, it's not
    # possible until such option is implemented in cmake.

    list(FILTER _src_list EXCLUDE REGEX "\\.pch$")

    # AAX
    if(HAVE_AAX AND NOT _pluginapi_lib STREQUAL "IPlug_AAX")
        list(FILTER _src_list EXCLUDE REGEX "^${IPLUG2_ROOT_PATH}/IPlug/AAX/.*\\.cpp$")
    endif()

    # APP
    if(NOT ${_pluginapi_lib} STREQUAL "IPlug_APP")
        list(FILTER _src_list EXCLUDE REGEX "^${IPLUG2_ROOT_PATH}/IPlug/APP/.*\\.cpp$")
    endif()

    # AUv2
    if(HAVESDK_AU AND NOT ${_pluginapi_lib} STREQUAL "IPlug_AU")
        list(FILTER _src_list EXCLUDE REGEX "^${IPLUG2_ROOT_PATH}/IPlug/AUv2/.*\\.(mm|c|cpp)$")
    endif()

    # AUv3
    if(HAVESDK_AUv3 AND NOT ${_pluginapi_lib} STREQUAL "IPlug_AUv3")
        list(FILTER _src_list EXCLUDE REGEX "^${IPLUG2_ROOT_PATH}/IPlug/AUv3/.*\\.(m|mm|cpp)$")
    endif()

    # VST2
    if(HAVESDK_VST2 AND NOT ${_pluginapi_lib} STREQUAL "IPlug_VST2")
        list(FILTER _src_list EXCLUDE REGEX "^${IPLUG2_ROOT_PATH}/IPlug/VST2/.*\\.cpp$")
    endif()

    # VST3
    if(HAVESDK_VST3 AND NOT ${_pluginapi_lib} STREQUAL "IPlug_VST3")
        list(FILTER _src_list EXCLUDE REGEX "^${IPLUG2_ROOT_PATH}/IPlug/VST3/.*\\.cpp$")
    endif()

    # WEB
    # if(NOT ${_pluginapi_lib} STREQUAL "IPlug_WEB")
    #     list(REMOVE_ITEM _src_list
    #         "${IPLUG2_ROOT_PATH}/IPlug/WEB/IPlugWAM.cpp"
    #         "${IPLUG2_ROOT_PATH}/IPlug/WEB/IPlugWeb.cpp"
    #     )
    # endif()

    # Add remaining source files
    target_sources(${_libName} PRIVATE ${_src_list})

    # Build the config definition list
    set(CONFIG_DEFINITIONS "")
    set(_def ${CONFIG_VARIABLES})
    list(REMOVE_ITEM _def ${_iplug_config_definition_exclude})
    list(REMOVE_DUPLICATES _def)
    foreach(_name IN LISTS _def)
        set(_value ${CONFIG_${_name}})
        if(DEFINED CONFIG_OVERRIDE_${_name})
            set(_value ${CONFIG_OVERRIDE_${_name}})
            unset(CONFIG_OVERRIDE_${_name} PARENT_SCOPE)
        endif()
        if(_name MATCHES "^RESOURCE_")
            # cmake_print_variables(_name)
            string(REPLACE "RESOURCE_" "" _name "${_name}")
        else()
            list(FIND _projectConfigArgs "${_name}" _result)
            if(_result EQUAL -1)
                unset(CONFIG_${_name} PARENT_SCOPE)
            endif()
        endif()
        if(NOT DEFINED _value OR _value STREQUAL "")
            continue()
        endif()
        list(FIND _iplug_config_string_variables "${_name}" _result)
        if(_result EQUAL -1)
            set(_result "${_name}=${_value}")
        else()
            set(_result "${_name}=\"${_value}\"")
        endif()
        list(APPEND CONFIG_DEFINITIONS "${_result}")
    endforeach()

    # iplug_print_lists(CONFIG_DEFINITIONS)

	# Add IPLUG2_STATIC definition when we're compiling the library
    target_compile_definitions(${_libName}
        PRIVATE
            IPLUG2_STATIC
        PUBLIC
            ${CONFIG_DEFINITIONS}
    )

    # Configure precompiled headers for the static library
    target_precompile_headers(${_libName}
        PUBLIC
            "${IPLUG2_ROOT_PATH}/IPlug/IPlugPCH.h"
    )

    target_link_libraries(${_libName}
        PRIVATE
            IPlug_CompileOptions
        PUBLIC
            IPlug_SharedLinkLibraries
            IPlug_SharedDefinitions
            IPlug_SharedIncludes
            ${_pluginapi_lib}
    )

    # add sources to the target if defined in project config
    if(CONFIG_SOURCES)
        target_sources(${_target} ${CONFIG_SOURCES})
    endif()

    # add include directories to the target if defined in project config
    if(CONFIG_INCLUDE_DIRECTORIES)
        target_include_directories(${_target} ${CONFIG_INCLUDE_DIRECTORIES})
    endif()

    # add include directories to the target if defined in project config
    if(CONFIG_LINK_LIBRARIES)
        target_link_libraries(${_target} ${CONFIG_INCLUDE_DIRECTORIES} "")
    endif()

    if(NOT IPLUG2_EXTERNAL_PROJECT)
        set_target_properties(${_libName} PROPERTIES FOLDER "Examples/${PROJECT_NAME}/Libraries")
    else()
        set_target_properties(${_libName} PROPERTIES FOLDER "Libraries")
    endif()
endfunction()


#------------------------------------------------------------------------------
