#pragma once
/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

/**
 * @file
 * @brief Used for choosing a drawing backend
 */

// clang-format off
#ifndef NO_IGRAPHICS
	#include "IGraphicsConstants.h"
	#include "IGraphicsStructs.h"
	#include "IGraphicsPopupMenu.h"
	#include "IGraphicsUtilities.h"
	#include "IGraphics.h"
	#include "IGraphicsPathBase.h"
	#include "IControl.h"
	#include "IControls.h"
	#include "IGraphicsLiveEdit.h"

	#ifndef DOXYGEN_GENERATING_OUTPUT
		#if defined IGRAPHICS_LICE
			#include "IGraphicsLice.h"
			#define IGRAPHICS_DRAW_CLASS_TYPE IGraphicsLice
			#if defined IGRAPHICS_GL || defined IGRAPHICS_METAL
				#error When using IGRAPHICS_LICE, don't define IGRAPHICS_METAL or IGRAPHICS_GL*
			#endif
		#elif defined IGRAPHICS_AGG
			#include "IGraphicsAGG.h"
			#define IGRAPHICS_DRAW_CLASS_TYPE IGraphicsAGG
			#if defined IGRAPHICS_GL || defined IGRAPHICS_METAL
				#error When using IGRAPHICS_AGG, don't define IGRAPHICS_METAL or IGRAPHICS_GL*
			#endif
		#elif defined IGRAPHICS_CAIRO
			#include "IGraphicsCairo.h"
			#define IGRAPHICS_DRAW_CLASS_TYPE IGraphicsCairo
			#if defined IGRAPHICS_GL || defined IGRAPHICS_METAL
				#error When using IGRAPHICS_CAIRO, don't define IGRAPHICS_METAL or IGRAPHICS_GL*
			#endif
		#elif defined IGRAPHICS_NANOVG
			#include "IGraphicsNanoVG.h"
			#define IGRAPHICS_DRAW_CLASS_TYPE IGraphicsNanoVG
		#elif defined IGRAPHICS_SKIA
			#include "IGraphicsSkia.h"
			#define IGRAPHICS_DRAW_CLASS_TYPE IGraphicsSkia
		#elif defined IGRAPHICS_CANVAS
			#include "IGraphicsCanvas.h"
			#define IGRAPHICS_DRAW_CLASS_TYPE IGraphicsCanvas
			#if defined IGRAPHICS_GL || defined IGRAPHICS_METAL
				#error When using IGRAPHICS_CANVAS, don't define IGRAPHICS_METAL or IGRAPHICS_GL*
			#endif
		#else
			#error NO IGRAPHICS_MODE defined
		#endif
	#endif

	namespace iplug::igraphics
	{
		using IGRAPHICS_DRAW_CLASS = IGRAPHICS_DRAW_CLASS_TYPE;
	}
	#undef IGRAPHICS_DRAW_CLASS_TYPE

	#if defined IGRAPHICS_IMGUI
		BEGIN_INCLUDE_DEPENDENCIES
		#include <imgui.h>
		END_INCLUDE_DEPENDENCIES
	#endif

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

	// Controls
	#include "IBubbleControl.h"
	#include "IColorPickerControl.h"
	#include "ICornerResizerControl.h"
	#include "IFPSDisplayControl.h"
	#include "ILEDControl.h"
	#include "IPopupMenuControl.h"
	#include "IRTTextControl.h"
	#include "ITextEntryControl.h"
	#include "IVDisplayControl.h"
	#include "IVKeyboardControl.h"
	#include "IVMeterControl.h"
	#include "IVMultiSliderControl.h"
	#include "IVScopeControl.h"
	#include "IVPresetManagerControl.h"
	#include "IVNumberBoxControl.h"
#endif
// clang-format on
