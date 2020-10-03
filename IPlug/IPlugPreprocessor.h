/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/


#pragma once

// clang-format off

//-----------------------------------------------------------------------------
// Global preprocessor definitions

#define PREPROCESSOR_TOKEN_STRING(expr)         #expr
#define PREPROCESSOR_TOKEN_CONCAT(A, B)         A##B
#define PREPROCESSOR_TOKEN_VARIADIC(...)        __VA_ARGS__
#define PREPROCESSOR_STRING(expr)               PREPROCESSOR_TOKEN_STRING(expr)
#define PREPROCESSOR_CONCAT(A, B)               PREPROCESSOR_TOKEN_CONCAT(A, B)
#define PREPROCESSOR_UNPARENTHESIZE(...)        PREPROCESSOR_TOKEN_VARIADIC __VA_ARGS__

// No quotes in filename
#define PLATFORM_HEADER(filename)               PREPROCESSOR_STRING(PLATFORM_NAME/filename)
#define PLATFORM_PREFIX_HEADER(filename)        PREPROCESSOR_STRING(PREPROCESSOR_CONCAT(PLATFORM_NAME/PLATFORM_NAME, filename))

#define BEGIN_IPLUG_NAMESPACE                   namespace iplug {
#define BEGIN_IGRAPHICS_NAMESPACE               namespace igraphics {
#define END_IPLUG_NAMESPACE                     }
#define END_IGRAPHICS_NAMESPACE                 }

#define WARNING_MESSAGE(msg)                    PRAGMA(message(__FILE__ "(" PREPROCESSOR_STRING(__LINE__) ") : " "WARNING: " msg))
#define REMINDER_MESSAGE(msg)                   PRAGMA(message(__FILE__ "(" PREPROCESSOR_STRING(__LINE__) "): " msg))
#define PRAGMA_MESSAGE(msg)                     PRAGMA(message(msg))

#define DEPRECATED(version, message)            [[deprecated(message)]]

#ifdef PARAMS_MUTEX
	#define ENTER_PARAMS_MUTEX                  mParams_mutex.Enter();        Trace(TRACELOC, "%s", "ENTER_PARAMS_MUTEX");
	#define LEAVE_PARAMS_MUTEX                  mParams_mutex.Leave();        Trace(TRACELOC, "%s", "LEAVE_PARAMS_MUTEX");
	#define ENTER_PARAMS_MUTEX_STATIC           _this->mParams_mutex.Enter(); Trace(TRACELOC, "%s", "ENTER_PARAMS_MUTEX");
	#define LEAVE_PARAMS_MUTEX_STATIC           _this->mParams_mutex.Leave(); Trace(TRACELOC, "%s", "LEAVE_PARAMS_MUTEX");
#else
	#define ENTER_PARAMS_MUTEX
	#define LEAVE_PARAMS_MUTEX
	#define ENTER_PARAMS_MUTEX_STATIC
	#define LEAVE_PARAMS_MUTEX_STATIC
#endif

// clang-format on

#ifndef IGRAPHICS_GL
	#if defined IGRAPHICS_GLES2 || IGRAPHICS_GLES3 || IGRAPHICS_GL2 || IGRAPHICS_GL3
		#define IGRAPHICS_GL
	#endif
#endif

#ifndef PLATFORM_NAME
	#error "PLATFORM_NAME must be defined. Make sure cmake declared this when generating project."
#endif

// Set non-active platforms to 0
#ifndef PLATFORM_WINDOWS
	#define PLATFORM_WINDOWS 0
#endif

#ifndef PLATFORM_IOS
	#define PLATFORM_IOS 0
#endif

#ifndef PLATFORM_MAC
	#define PLATFORM_MAC 0
#endif

#ifndef PLATFORM_LINUX
	#define PLATFORM_LINUX 0
#endif

#ifndef PLATFORM_WEB
	#define PLATFORM_WEB 0
#endif

#if PLATFORM_WINDOWS + PLATFORM_IOS + PLATFORM_MAC + PLATFORM_LINUX + PLATFORM_WEB != 1
	#error "One and only one platform should be active. Check cmake settings."
#endif

// Default floating-point type to use for variables and functions unless explicitly specified
#ifndef IPLUG2_TFLOAT_TYPE
	#define IPLUG2_TFLOAT_TYPE float
#endif

#ifdef __OBJC__
	#import <Cocoa/Cocoa.h>

	#if defined(VST2_API)
		#define API_SUFFIX _vst
	#elif defined(AU_API)
		#define API_SUFFIX _au
	#elif defined(AUv3_API)
		#define API_SUFFIX _auv3
	#elif defined(AAX_API)
		#define API_SUFFIX _aax
	#elif defined(VST3_API)
		#define API_SUFFIX _vst3
	#elif defined(APP_API)
		#define API_SUFFIX _app
	#endif

	#define IGRAPHICS_VIEW          IGraphicsView_vIPLUG2_##API_SUFFIX
	#define IGRAPHICS_MENU          IGraphicsMenu_vIPLUG2_##API_SUFFIX
	#define IGRAPHICS_MENU_RCVR     IGraphicsMenuRcvr_vIPLUG2_##API_SUFFIX
	#define IGRAPHICS_FORMATTER     IGraphicsFormatter_vIPLUG2_##API_SUFFIX
	#define IGRAPHICS_TEXTFIELD     IGraphicsTextField_vIPLUG2_##API_SUFFIX
	#define IGRAPHICS_TEXTFIELDCELL IGraphicsTextFieldCell_vIPLUG2_##API_SUFFIX
	#define IGRAPHICS_GLLAYER       IGraphicsLayer_vIPLUG2_##API_SUFFIX
	#define IGRAPHICS_IMGUIVIEW     IGraphicsImGuiView_vIPLUG2_##API_SUFFIX
	#define MNVGtexture             MNVGtexture_vIPLUG2_##API_SUFFIX
	#define MNVGbuffers             MNVGbuffers_vIPLUG2_##API_SUFFIX
	#define MNVGcontext             MNVGcontext_vIPLUG2_##API_SUFFIX

	#undef API_SUFFIX
#endif

#ifndef PLUG_VERSION_STR
	#error You need to define PLUG_VERSION_STR in config.h - A string to identify the version number
#endif

#ifndef PLUG_URL_STR
	#define PLUG_URL_STR ""
#endif

#ifndef PLUG_EMAIL_STR
	#define PLUG_EMAIL_STR ""
#endif

#ifndef PLUG_COPYRIGHT_STR
	#define PLUG_COPYRIGHT_STR ""
#endif

#ifndef PLUG_VERSION_HEX
	#error You need to define PLUG_VERSION_HEX in config.h - The hexadecimal version number in the form 0xVVVVRRMM: V = version, R = revision, M = minor revision.
#endif

#ifndef PLUG_UNIQUE_ID
	#error You need to define PLUG_UNIQUE_ID in config.h - The unique four char ID for your plug-in, e.g. 'IPeF'
#endif

#ifndef PLUG_MFR_ID
	#error You need to define PLUG_MFR_ID in config.h - The unique four char ID for your manufacturer, e.g. 'Acme'
#endif

// TODO: Remove arbitrary size limits. Let people with their 8K or 16K screens be able to see without magnifying glass
//       Should be a normalized float value (0.0-1.0) representing 0-100% of current monitor resolution
#ifndef PLUG_MAX_WIDTH
	#define PLUG_MAX_WIDTH (PLUG_WIDTH * 2)
#endif

#ifndef PLUG_MAX_HEIGHT
	#define PLUG_MAX_HEIGHT (PLUG_HEIGHT * 2)
#endif

// TODO: FPS should be read from client hardware and then have a FPS limit option client side
#ifndef PLUG_FPS
	#define PLUG_FPS 60
#endif

#ifdef AAX_API
	#ifndef AAX_TYPE_IDS
		#error AAX_TYPE_IDS not defined - list of comma separated four char IDs, that correspond to the different possible channel layouts of your plug-in, e.g. 'EFN1', 'EFN2'
	#endif

	#ifndef AAX_PLUG_MFR_STR
		#error AAX_PLUG_MFR_STR not defined - The manufacturer name as it will appear in Pro tools preset manager
	#endif

	#ifndef AAX_PLUG_NAME_STR
		#error AAX_PLUG_NAME_STR not defined - The plug-in name string, which may include shorten names separated with newline characters, e.g. "IPlugEffect\nIPEF"
	#endif

	#ifndef AAX_PLUG_CATEGORY_STR
		#error AAX_PLUG_CATEGORY_STR not defined - String defining the category for your plug-in, e.g. "Effect"
	#endif

	#if AAX_DOES_AUDIOSUITE
		#ifndef AAX_TYPE_IDS_AUDIOSUITE
			#error AAX_TYPE_IDS_AUDIOSUITE not defined - list of comma separated four char IDs, that correspond to the different possible channel layouts of your plug-in when running off-line in audio suite mode, e.g. 'EFA1', 'EFA2'
		#endif
	#endif
#endif

#define API_EXT2
#if VST2_API
	#define API_EXT "vst"
	#if REAPER_PLUGIN
		#define LICE_PROVIDED_BY_APP
		#define PLUGIN_API_BASE IPlugReaperVST2
		#undef FillRect
		#undef DrawText
		#undef Polygon
	#endif
#elif AU_API
	#define API_EXT "audiounit"
#elif AUv3_API
	#undef API_EXT2
	#define API_EXT  "app"
	#define API_EXT2 ".AUv3"
#elif AAX_API
	#define API_EXT "aax"
	#define PROTOOLS
#elif APP_API
	#define API_EXT "app"
#elif VST3_API || VST3C_API || VST3P_API
	#define API_EXT "vst3"
	#if defined VST3C_API
		#undef PLUG_CLASS_NAME
		#define PLUG_CLASS_NAME VST3Controller
	#elif defined VST3P_API
		#undef NO_IGRAPHICS
		#define NO_IGRAPHICS
	#endif
#endif

#if PLATFORM_WINDOWS || PLATFORM_WEB
	#define BUNDLE_ID ""
#elif PLATFORM_MAC
	#define BUNDLE_ID BUNDLE_DOMAIN "." BUNDLE_MFR "." API_EXT "." BUNDLE_NAME API_EXT2
#elif PLATFORM_IOS
	#define BUNDLE_ID BUNDLE_DOMAIN "." BUNDLE_MFR "." BUNDLE_NAME API_EXT2
#elif PLATFORM_LINUX
// TODO:
#endif
