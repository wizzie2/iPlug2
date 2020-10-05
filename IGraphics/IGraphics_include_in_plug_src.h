/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once


/**
 * @file IGraphics_include_in_plug_src.h
 * @brief IGraphics source include
 * Include this file in the main cpp file if using IGraphics outside a plugin context
 */

#if PLATFORM_WINDOWS
	#define IGRAPHICS IGraphicsWin
#elif PLATFORM_MAC
	#define IGRAPHICS IGraphicsMac
#elif PLATFORM_IOS
	#define IGRAPHICS IGraphicsIOS
#elif PLATFORM_WEB
	#define IGRAPHICS IGraphicsWeb
#endif

IGRAPHICS* gGraphics = nullptr;
extern void* gHINSTANCE;

#if PLATFORM_WEB
void StartMainLoopTimer()
{
	IGraphicsWeb* pGraphics = gGraphics;
	emscripten_set_main_loop(pGraphics->OnMainLoopTimer, 0 /*pGraphics->FPS()*/, 1);
}
#endif

namespace iplug::igraphics
{
	IGRAPHICS* MakeGraphics(IGEditorDelegate& dlg,
							int w   = Config::plugWidth,
							int h   = Config::plugHeight,
							int fps = Config::plugFPS)
	{
		IGRAPHICS* pGraphics = new IGRAPHICS(dlg, w, h, fps);
		pGraphics->SetWinModuleHandle(gHINSTANCE);
		pGraphics->SetBundleID(Config::bundleID);
		pGraphics->SetSharedResourcesSubPath(Config::sharedResourcesSubpath);
		if constexpr (EPlatform::Native == EPlatform::Web)
		{
			gGraphics = pGraphics;
			return gGraphics;
		}
		else
			return pGraphics;
	}


}  // namespace iplug::igraphics

#undef IGRAPHICS
