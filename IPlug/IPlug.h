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
	#if VST3C_API
	#elif VST3P_API
	#else
	#endif
	#include <pluginterfaces/base/ibstream.h>
	#include <pluginterfaces/base/keycodes.h>
	#include <pluginterfaces/base/ustring.h>
	#include "pluginterfaces/vst/ivstcomponent.h"
	#include "pluginterfaces/vst/ivsteditcontroller.h"
	#include "pluginterfaces/vst/ivstmidicontrollers.h"
	#include <pluginterfaces/vst/ivstchannelcontextinfo.h>
	#include <pluginterfaces/vst/ivstcontextmenu.h>
	#include <pluginterfaces/vst/ivstevents.h>
	#include <pluginterfaces/vst/ivstparameterchanges.h>
	#include <pluginterfaces/vst/ivstprocesscontext.h>
	#include <pluginterfaces/vst/vstspeaker.h>
	#include <pluginterfaces/vst/vsttypes.h>
	#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>
	#include <public.sdk/source/vst/vstaudioeffect.h>
	#include <public.sdk/source/vst/vstbus.h>
	#include <public.sdk/source/vst/vsteditcontroller.h>
	#include <public.sdk/source/vst/vsteventshelper.h>
	#include <public.sdk/source/vst/vstparameters.h>
	#include <public.sdk/source/vst/vstsinglecomponenteffect.h>
	#include <public.sdk/source/vst/hosting/parameterchanges.h>
	#include "public.sdk/source/main/pluginfactory.h"
#endif
END_INCLUDE_DEPENDENCIES

#undef stricmp
#undef strnicmp


//-----------------------------------------------------------------------------
namespace iplug
{
	class IPLUG2_PLUGIN;  // Fwd
	using Plugin = IPLUG2_PLUGIN;

	// Located in IPlug.cpp temporarily
	const int GetScaleForHWND(const HWND hWnd, const bool useCachedResult = true);

}  // namespace iplug
