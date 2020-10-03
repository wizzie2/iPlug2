/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once


/**
 * @file IGraphics_include_in_plug_hdr.h
 * @brief IGraphics header include
 * Include this file in the main header if using IGraphics outside a plugin context
 */

//BEGIN_INCLUDE_DEPENDENCIES
//#ifndef NO_IGRAPHICS
//	#if defined IGRAPHICS_GLES2
//		#if PLATFORM_IOS
//			#include <OpenGLES/ES2/gl.h>
//		#elif PLATFORM_WEB
//			#include <GLES2/gl2.h>
//		#endif
//	#elif defined IGRAPHICS_GLES3
//		#if PLATFORM_IOS
//			#include <OpenGLES/ES3/gl.h>
//		#elif PLATFORM_WEB
//			#include <GLES3/gl3.h>
//		#endif
//	#elif defined IGRAPHICS_GL2 || defined IGRAPHICS_GL3
//		#if PLATFORM_WINDOWS
//			#include <glad.h>
//		#elif PLATFORM_MAC
//			#if defined IGRAPHICS_GL2
//				#include <OpenGL/gl.h>
//			#elif defined IGRAPHICS_GL3
//				#include <OpenGL/gl3.h>
//			#endif
//		#else
//			#include <OpenGL/gl.h>
//		#endif
//	#endif
//#endif  // NO_IGRAPHICS
//END_INCLUDE_DEPENDENCIES

//#ifndef NO_IGRAPHICS
//	#if PLATFORM_WINDOWS
//		#include "Platforms/IGraphicsWin.h"
//	#elif PLATFORM_MAC
//		#include "Platforms/IGraphicsMac.h"
//	#elif PLATFORM_IOS
//		#include "Platforms/IGraphicsIOS.h"
//	#elif PLATFORM_LINUX
//		#include "Platforms/IGraphicsLinux.h"
//	#elif PLATFORM_WEB
//		#include "Platforms/IGraphicsWeb.h"
//	#endif
//#endif  // NO_IGRAPHICS
