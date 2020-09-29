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
