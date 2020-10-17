#pragma once

/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/


/**
 * @file
 * @brief IPlug2 core header
 */

#include "PAL/Platform.h"  // Needs to be first
#include "PAL/Type.h"
#include "PAL/System.h"


//-----------------------------------------------------------------------------
// Required third-party headers

BEGIN_INCLUDE_DEPENDENCIES
#include <WDL/wdlendian.h>
#include <WDL/wdlstring.h>
#include <WDL/queue.h>
#include <WDL/mutex.h>
#include <WDL/ptrlist.h>

#if VST2_API
#elif AU_API
#elif AUv3_API
#elif AAX_API
#elif APP_API
	#include <RtAudio.h>
	#include <RtMidi.h>
#elif WAM_API
#elif WEB_API
#elif VST3_API
#endif
END_INCLUDE_DEPENDENCIES

namespace iplug
{
	// Fwd
	class IPLUG2_PLUGIN;
}  // namespace iplug

#ifndef IPLUG2_STATIC
using Plugin = iplug::IPLUG2_PLUGIN;
#endif

//-----------------------------------------------------------------------------
namespace iplug
{
	// Located in IPlug.cpp temporarily
	const int GetScaleForHWND(const HWND hWnd, const bool useCachedResult = true);

}  // namespace iplug
