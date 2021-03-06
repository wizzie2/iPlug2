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
#define DEBUGBREAK()             __debugbreak()


//-----------------------------------------------------------------------------
// Windows specific types

namespace iplug::type
{
	struct Windows : iplug::generic::Types
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
	using Platform = Windows;
}  // namespace iplug::types


//-----------------------------------------------------------------------------
// Configure and include windows.h
// TODO: Remove this include file when isolation is complete

#include "WindowsSDK.h"
