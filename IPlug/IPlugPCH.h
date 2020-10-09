#pragma once

/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/

/**
 * @file
 * @brief IPlug2 Precompiled headers
 */

#include "PAL/Platform.h"  // Needs to be first

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <codecvt>
#include <complex>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <version>

#if __cpp_lib_bit_cast
	#include <bit>
#endif

BEGIN_INCLUDE_DEPENDENCIES
#include <WDL/wdlendian.h>
#include <WDL/wdlstring.h>
#include <WDL/queue.h>
#include <WDL/mutex.h>
#include <WDL/ptrlist.h>

#if VST3_API
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

#include "PAL/Type.h"
#include "PAL/System.h"
#include "IPlugConstants.h"
#include "IPlugLogger.h"
#include "IPlugMath.h"
#include "IPlugStructs.h"
#include "IPlugUtilities.h"
#include "IPlugMidi.h"
#include "IPlugQueue.h"
#include "IPlugPaths.h"
#include "IPlugTimer.h"
#include "IPlugParameter.h"
#include "IPlugDelegate_select.h"
#include "IPlugEditorDelegate.h"
#include "IPlugPluginBase.h"
#include "IPlugAPIBase.h"
#include "IPlugProcessor.h"
#include "ISender.h"
#include "Extras/Easing.h"
#ifndef NO_IGRAPHICS
	#include "IGraphicsConstants.h"
	#include "IGraphicsStructs.h"
	#include "IGraphicsPopupMenu.h"
	#include "IGraphicsUtilities.h"
	#include "IGraphics.h"
	#include "IGraphics_select.h"
	#include "IControl.h"
	#include "IControls.h"
	#include "IBubbleControl.h"
	#include "IColorPickerControl.h"
	#include "ICornerResizerControl.h"
	#include "IFPSDisplayControl.h"
	#include "IGraphicsLiveEdit.h"
	#include "ILEDControl.h"
	#include "IPopupMenuControl.h"
	#include "IRTTextControl.h"
	#include "ITextEntryControl.h"
	#include "IVDisplayControl.h"
	#include "IVKeyboardControl.h"
	#include "IVMeterControl.h"
	#include "IVMultiSliderControl.h"
	#include "IVScopeControl.h"

	#if PLATFORM_WINDOWS
		#include "Platforms/IGraphicsWin.h"
	#elif PLATFORM_MAC
		#include "Platforms/IGraphicsMac.h"
	#elif PLATFORM_IOS
		#include "Platforms/IGraphicsIOS.h"
	#elif PLATFORM_LINUX
		#include "Platforms/IGraphicsLinux.h"
	#elif PLATFORM_WEB
		#include "Platforms/IGraphicsWeb.h"
	#endif
#endif

#if VST2_API
	#define IPLUG2_PLUGIN IPlugVST2
	#ifdef REAPER_PLUGIN
		#include "VST2/IPlugReaperVST2.h"
	#else
		#include "VST2/IPlugVST2.h"
	#endif
#elif AU_API
	#include "AUv2/IPlugAU.h"
	#define IPLUG2_PLUGIN IPlugAU;
#elif AUv3_API
	#include "AUv3/IPlugAUv3.h"
	#define IPLUG2_PLUGIN IPlugAUv3;
#elif AAX_API
	#include "AAX/IPlugAAX.h"
	#define IPLUG2_PLUGIN IPlugAAX;
#elif APP_API
	#include "APP/IPlugAPP.h"
	#define IPLUG2_PLUGIN IPlugAPP;
#elif WAM_API
	#include "WEB/IPlugWAM.h"
	#define IPLUG2_PLUGIN IPlugWAM;
#elif WEB_API
	#include "WEB/IPlugWeb.h"
	#define IPLUG2_PLUGIN IPlugWeb;
#elif VST3C_API
	#include "VST3/IPlugVST3_Controller.h"
	#define IPLUG2_PLUGIN IPlugVST3Controller;
#elif VST3P_API
	#include "VST3/IPlugVST3_Processor.h"
	#define IPLUG2_PLUGIN IPlugVST3Processor;
#elif VST3_API
	#include "VST3/IPlugVST3.h"
	#define IPLUG2_PLUGIN IPlugVST3;
#else
	#error "No API defined!"
#endif

#ifndef IPLUG2_STATIC
using Plugin = iplug::IPLUG2_PLUGIN;
#endif

#undef IPLUG2_PLUGIN
