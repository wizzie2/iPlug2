/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/

#pragma once


//-----------------------------------------------------------------------------
// Platform configuration

#define PLATFORM_LITTLE_ENDIAN   1
#define PLATFORM_CACHE_LINE_SIZE 64
#define DEBUGBREAK()             __emit__(0x90CC)


//-----------------------------------------------------------------------------
// Linux specific types

namespace iplug::type
{
	struct Linux : iplug::generic::Types
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
	using Platform = Linux;
}  // namespace iplug::type


//-----------------------------------------------------------------------------
BEGIN_INCLUDE_DEPENDENCIES
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#if !NO_IGRAPHICS
	#include <OpenGL/gl.h>
#endif

#include <WDL/swell/swell.h>
END_INCLUDE_DEPENDENCIES

#undef LineTo
#undef SetPixel
#undef FillRect
#undef DrawText
#undef Polygon
#undef SetCursorPos
