/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/

#pragma once


//---------------------------------------------------------
// Platform configuration

#define PLATFORM_LITTLE_ENDIAN   1
#define PLATFORM_CACHE_LINE_SIZE 64
#define DEBUGBREAK()             __debugbreak()


//---------------------------------------------------------
// WEB specific types

namespace iplug::type
{
	struct Web : iplug::generic::Types
	{
		using uint8  = std::uint8_t;
		using uint16 = std::uint16_t;
		using uint32 = std::uint32_t;
		using uint64 = std::uint64_t;
		using int8   = std::int8_t;
		using int16  = std::int16_t;
		using int32  = std::int32_t;
		using int64  = std::int64_t;
	};
	using Platform = Web;
}  // namespace iplug::type


//-----------------------------------------------------------------------------

BEGIN_INCLUDE_DEPENDENCIES
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>

#if !NO_IGRAPHICS
	#if defined IGRAPHICS_GLES2
		#include <GLES2/gl2.h>
	#elif defined IGRAPHICS_GLES3
		#include <GLES3/gl3.h>
	#else
		#include <OpenGL/gl.h>
	#endif
#endif
END_INCLUDE_DEPENDENCIES
