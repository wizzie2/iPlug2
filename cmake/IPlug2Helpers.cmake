include_guard(GLOBAL)

option(IPLUG2_DEVELOPER_CMAKE_DEBUG "Debug output for Iplug2 cmake scripts" OFF)

#------------------------------------------------------------------------------
# helper macros

macro(iplug_set_caller)
    set(_caller ${_caller_override})
    if(NOT _caller)
        set(_caller ${CMAKE_CURRENT_FUNCTION})
    endif()
endmacro()

macro(iplug_syntax_error)
    iplug_set_caller()
    foreach(_str ${ARGN})
        message(STATUS "[ FAIL ]\t<${_caller}> ${_str}")
    endforeach()
    message(FATAL_ERROR "?SYNTAX ERROR\n")
endmacro()

macro(iplug_debug_message)
    if(IPLUG2_DEVELOPER_CMAKE_DEBUG)
        set(_msg ${ARGN})
        iplug_set_caller()
        message(STATUS "[  OK  ]\t<${_caller}> ${_msg}")
    endif()
endmacro()

macro(iplug_info)
    set(_msg ${ARGN})
    iplug_set_caller()
    message(STATUS "[ INFO ]\t<${_caller}> ${_msg}")
endmacro()

macro(iplug_warning)
    set(_msg ${ARGN})
    iplug_set_caller()
    message(STATUS "[ WARN ]\t<${_caller}> ${_msg}")
endmacro()

#------------------------------------------------------------------------------
# iplug_print_list
# debug helper to show content of a list
function(iplug_print_lists)
    cmake_parse_arguments(_arg "" "" "" ${ARGN})
    set(_lists ${_arg_UNPARSED_ARGUMENTS})
    foreach(_list IN ITEMS ${_lists})
        set(index 0)
        foreach(element IN LISTS ${_list})
            message("${_list} [${index}]=\"${element}\"")
            MATH(EXPR index "${index}+1")
        endforeach()
    endforeach()
endfunction()

#------------------------------------------------------------------------------
# string_assert
#
# Aborts script execution with fatal error message if string is empty
#
#     _str - String to test
#     _msg - Message to write if (_str) is empty
#
function(string_assert _str _msg)
	set(SYNTAX_ERROR "${_str}-Guru Meditation")
	if(_str STREQUAL "NOTFOUND" OR SYNTAX_ERROR STREQUAL "-Guru Meditation")
        iplug_syntax_error("${_msg}")
    endif()
    iplug_debug_message("${_str}")
endfunction()


#------------------------------------------------------------------------------
# validate_path
function(validate_path _path _msg)
    if(NOT _path OR NOT IS_DIRECTORY "${_path}" )
        iplug_syntax_error("${_msg}")
    endif()
    iplug_debug_message("${_path}")
endfunction()


#------------------------------------------------------------------------------
# iplug_validate_string(<variable name>)
#   Reads the string value from a variable passed as argument to the function and
#   validates if its conformant to specified options.
#
#   [DEFINED]               Only validates if the variable is defined.
#   [DEFAULT <string>]      If variable is empty or undefined, set it to <string> in PARENT_SCOPE.
#   [NOTEMPTY]              Not allowed to be empty.
#   [FILE_EXISTS]           Refering to an existing file.
#   [PATH_EXISTS]           Refering to an existing path.
#   [ABSOLUTE]              Stores the absolute path of path/file to specified variable in PARENT_SCOPE.
#   [ALPHAFIRST]            First character must be an alphabetic character.
#   [SINGLE_QUOTED]         Required to be surrounded by single quote marks.
#   [PREFIX <prefix>]       Prepend <prefix>_ to the variable name from which the string value is read.
#   [SUFFIX <suffix>]       Append _<suffix> to the variable name from which the string value is read.
#   [MINLENGTH <length>]    Minimum length allowed. Ignoring apostophes if [SINGLE_QUOTED] is set.
#   [MAXLENGTH <length>]    Maximum length allowed. Ignoring apostophes if [SINGLE_QUOTED] is set.
#   [MINVALUE <min>]        Minimum numeric integral or floating point value allowed.
#   [MAXVALUE <max>]        Maximum numeric integral or floating point value allowed.
#   [STREQUAL <string>...]  Required to be equal to one of the provided strings.
#   [VERSION <n>]           Valid version format of <n> amount of numbers and a dot between each number.
#                           (Implies [DOT] if testing for invalid characters)
#
#   If any of the following options are set, the function will validate if the string value is allowed
#   to contain specified characters. If none of the options are set, it will skip character testing.
#       [ALPHA]             A-Z (case insensitive)
#       [NUMERIC]           0-9
#       [SPACE]             Spaces
#       [HYPHEN]            -
#       [DOT]               .
#       [SLASH]             /
#       [APOSTROPHE]        '
#       [COMMA]             ,
#       [DELIMITER]         |
#       [UNDERSCORE]        _

function(iplug_validate_string _variable)
    set(_options      DEFINED NOTEMPTY FILE_EXISTS PATH_EXISTS ABSOLUTE ALPHAFIRST SINGLE_QUOTED)
    set(_char_options ALPHA NUMERIC SPACE HYPHEN DOT SLASH APOSTROPHE COMMA DELIMITER UNDERSCORE)
    set(_onevalue     PREFIX SUFFIX DEFAULT MINLENGTH MAXLENGTH VERSION MINVALUE MAXVALUE)
    set(_multivalue   STREQUAL)
    cmake_parse_arguments(OPTION "${_options};${_char_options}" "${_onevalue}" "${_multivalue}" ${ARGN})
    if(DEFINED OPTION_UNPARSED_ARGUMENTS)
        iplug_syntax_error("Unknown arguments \"${OPTION_UNPARSED_ARGUMENTS}\"")
    endif()

    set(_chars     "")
    set(_esc_chars "")
    set(_var "${_variable}")
    if(DEFINED OPTION_PREFIX)
        string(PREPEND _var "${OPTION_PREFIX}_")
    endif()

    if(DEFINED OPTION_SUFFIX)
        string(APPEND _var "_${OPTION_SUFFIX}")
    endif()

    if(OPTION_DEFINED AND NOT DEFINED ${_var})
        return()
    endif()

    if(DEFINED OPTION_DEFAULT AND ${_var} STREQUAL "")
        set(${_var} ${OPTION_DEFAULT})
        set(${_var} ${OPTION_DEFAULT} PARENT_SCOPE)
    endif()

    set(_value "${${_var}}")
    set(_defined "${_variable} = \"${_value}\"")

    if(OPTION_SINGLE_QUOTED)
        if(NOT ${_value} MATCHES "^'.*'$")
            iplug_syntax_error("${_defined}. Must be surrounded by single quote marks.")
        endif()
        string(REGEX REPLACE "^'(.*)'$" "\\1" _value "${_value}")
    endif()

    string(LENGTH "${_value}" _len)

    if(OPTION_NOTEMPTY AND _len EQUAL 0)
        iplug_syntax_error("${_defined}. Can not be an empty string.")
    elseif(_len EQUAL 0)
        return()
    endif()

    if(DEFINED OPTION_MINLENGTH AND _len LESS OPTION_MINLENGTH)
        iplug_syntax_error("${_defined}. Length is less than ${OPTION_MINLENGTH} characters.")
    endif()

    if(DEFINED OPTION_MAXLENGTH AND _len GREATER OPTION_MAXLENGTH)
        iplug_syntax_error("${_defined}. Length exceedes ${OPTION_MAXLENGTH} characters.")
    endif()

    if(DEFINED OPTION_MINVALUE OR DEFINED OPTION_MAXVALUE)
        if(NOT "${_value}" MATCHES "^[\\+-]?[0-9]*\\.?[0-9]+$")
            iplug_syntax_error("${_defined}. Must be a numeric value.")
        endif()
        if(DEFINED OPTION_MINVALUE AND _value LESS OPTION_MINVALUE)
            iplug_syntax_error("${_defined}. Value must be equal or greater than '${OPTION_MINVALUE}'.")
        endif()
        if(DEFINED OPTION_MAXVALUE AND _value GREATER OPTION_MAXVALUE)
            iplug_syntax_error("${_defined}. Value must be equal or less than '${OPTION_MAXVALUE}'.")
        endif()
        return()
    endif()

    if(DEFINED OPTION_VERSION)
        set(_regex "^[^\\.]+")
        set(_validstr "##" )
        if(OPTION_VERSION LESS 1 OR OPTION_VERSION GREATER 8)  # Maybe up to 8 version numbers is a thing somewhere?
            iplug_syntax_error("Invalid configuration VERSION=${OPTION_VERSION}. VERSION must be between 1 and 8.")
        endif()
        while(OPTION_VERSION GREATER_EQUAL 1)
            if(NOT OPTION_VERSION EQUAL 1)
                string(APPEND _regex "\\.[^\\.]+")
                string(APPEND _validstr ".##")
            endif()
            MATH(EXPR OPTION_VERSION "${OPTION_VERSION}-1")
        endwhile()
        string(APPEND _regex "$")
        if(NOT ${_value} MATCHES "${_regex}")
            iplug_syntax_error("${_defined}. Invalid version format. Requires \"${_validstr}\".")
        endif()
    endif()

    if(DEFINED OPTION_STREQUAL)
        foreach(_str IN LISTS OPTION_STREQUAL)
            if("${_value}" STREQUAL "${_str}")
                return()
            endif()
        endforeach()
        string(REPLACE ";" "\", \"" _str "${OPTION_STREQUAL}")
        iplug_syntax_error("${_defined} is invalid. Valid options are \"${_str}\".")
    endif()

    if(OPTION_FILE_EXISTS OR OPTION_PATH_EXISTS)
        if(NOT _len EQUAL 0)
            get_filename_component(_file "${_value}" ABSOLUTE)
            if(OPTION_ABSOLUTE)
                set(${_var} "${_file}")
                set(${_var} "${_file}" PARENT_SCOPE)
            endif()
            if(OPTION_FILE_EXISTS AND (IS_DIRECTORY "${_file}" OR NOT EXISTS "${_file}"))
                iplug_syntax_error("${_defined}. File does not exist.")
            endif()
            if(OPTION_PATH_EXISTS AND (NOT IS_DIRECTORY "${_file}" OR NOT EXISTS "${_file}"))
                iplug_syntax_error("${_defined}. Path does not exist.")
            endif()
        endif()
        return()
    endif()

    if(OPTION_ALPHAFIRST)
        if(${_value} MATCHES "^[^A-Za-z]")
            iplug_syntax_error("${_defined}. Must begin with an alphabetic character.")
        endif()
    endif()

    set(_noinvalid FALSE)
    foreach(_val IN LISTS _char_options)
        if(OPTION_${_val})
            set(_noinvalid TRUE)
            break()
        endif()
    endforeach()

    if(_noinvalid)
        if(OPTION_ALPHA)
            string(APPEND _chars "A-Za-z")
        endif()

        if(OPTION_NUMERIC)
            string(APPEND _chars "0-9")
        endif()

        if(OPTION_SPACE)
            string(APPEND _chars " ")
        endif()

        if(OPTION_SLASH)
            string(APPEND _esc_chars "\\/")
        endif()

        if(OPTION_DOT OR DEFINED OPTION_VERSION)
            string(APPEND _esc_chars "\\.")
        endif()

        if(OPTION_HYPHEN)
            string(APPEND _esc_chars "-")
        endif()

        if(OPTION_APOSTROPHE)
            string(APPEND _esc_chars "'")
        endif()

        if(OPTION_COMMA)
            string(APPEND _esc_chars ",")
        endif()

        if(OPTION_DELIMITER)
            string(APPEND _esc_chars "|")
        endif()

        if(OPTION_UNDERSCORE)
            string(APPEND _esc_chars "_")
        endif()

        set(_regex "[^${_chars}\\${_esc_chars}]")
        iplug_debug_message("${_variable}=\"${_value}\" ${_regex}")
        if(${_value} MATCHES "${_regex}")
            iplug_syntax_error("${_defined}. Contains invalid characters.")
        endif()
    endif()
endfunction()
