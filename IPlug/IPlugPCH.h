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


// std headers
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
#include <fstream>
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
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include <version>
#if __cpp_lib_bit_cast
	#include <bit>
#endif

// IPlug
#include "IPlug.h"
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
#if __has_include(IPLUG2_API_HEADER)
	#include IPLUG2_API_HEADER
#endif

// IGraphics
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
#endif
