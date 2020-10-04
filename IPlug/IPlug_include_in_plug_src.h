/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file IPlug_include_in_plug_src.h
 * @brief IPlug source include
 * Include this file in the main source for your plugin, after #including the main header for your plugin.
 * A preprocessor macro for a particular API such as VST2_API should be defined at project level
 * Depending on the API macro defined, a different entry point and helper methods are activated
 */

void* gHINSTANCE = nullptr;

// clang-format off

#if PLATFORM_WINDOWS && !VST3C_API

	#if VST2_API || AAX_API
	BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID res)
	{
		gHINSTANCE = hDllInst;
		return true;
	}
	#endif


	// TODO: move function. This function doesn't belong in an api header include, it should be something like PAL::System::GetDPI()
	const int GetScaleForHWND(const HWND hWnd, const bool useCachedResult=true)
	{
		static int CachedScale = 0;
		if(useCachedResult == false || CachedScale == 0)
			CachedScale = math::IntegralDivide(GetDpiForWindow(hWnd), USER_DEFAULT_SCREEN_DPI);
		return CachedScale;
	}
#endif


#if VST2_API
	extern "C"
	{
		IPLUG_EXPORT void* VSTPluginMain(audioMasterCallback hostCallback)
		{
			using namespace iplug;

			IPlugVST2* pPlug = MakePlug(InstanceInfo {hostCallback});

			if (pPlug)
			{
				AEffect& aEffect = pPlug->GetAEffect();
				pPlug->EnsureDefaultPreset();
				aEffect.numPrograms = std::max(aEffect.numPrograms, 1);  // some hosts don't like 0 presets
				return &aEffect;
			}
			return 0;
		}

		IPLUG_EXPORT int main(int hostCallback)
		{
		#if PLATFORM_MAC
			return (VstIntPtr) VSTPluginMain((audioMasterCallback) hostCallback);
		#else
			return (int) VSTPluginMain((audioMasterCallback) hostCallback);
		#endif
		}
	};

#elif VST3_API
	#if !defined VST3_PROCESSOR_UID && !defined VST3_CONTROLLER_UID
		#define VST3_PROCESSOR_UID  0xF2AEE70D, 0x00DE4F4E, PLUG_MFR_ID, PLUG_UNIQUE_ID
		#define VST3_CONTROLLER_UID 0xF2AEE70E, 0x00DE4F4F, PLUG_MFR_ID, PLUG_UNIQUE_ID
	#endif

	#if !VST3C_API
		bool InitModule()
		{
		#if PLATFORM_WINDOWS
			extern void* moduleHandle;
			gHINSTANCE = (HINSTANCE) moduleHandle;
		#endif
			return true;
		}

		// called after library is unloaded
		bool DeinitModule()
		{
			return true;
		}
	#endif

	#if !VST3P_API
		static Steinberg::FUnknown* createInstance(void*)
		{
			return (Steinberg::Vst::IAudioProcessor*) MakePlug(InstanceInfo());
		}

		BEGIN_FACTORY_DEF(PLUG_MFR, PLUG_URL_STR, PLUG_EMAIL_STR)
			DEF_CLASS2(INLINE_UID_FROM_FUID(FUID(VST3_PROCESSOR_UID)),
					   Steinberg::PClassInfo::kManyInstances,  // cardinality
					   kVstAudioEffectClass,                   // the component category (don't change this)
					   PLUG_NAME,                              // plug-in name
					   Steinberg::Vst::kSimpleModeSupported,   // means gui and plugin aren't split
					   VST3_SUBCATEGORY,                       // Subcategory for this plug-in
					   PLUG_VERSION_STR,                       // plug-in version
					   kVstVersionString,                      // the VST 3 SDK version (don't change - use define)
					   createInstance)                         // function pointer called to be instantiate
		END_FACTORY
	#else
		static Steinberg::FUnknown* createProcessorInstance(void*)
		{
			return MakeProcessor();
		}

		static Steinberg::FUnknown* createControllerInstance(void*)
		{
			return MakeController();
		}

		BEGIN_FACTORY_DEF(PLUG_MFR, PLUG_URL_STR, PLUG_EMAIL_STR)

		DEF_CLASS2(INLINE_UID_FROM_FUID(FUID(VST3_PROCESSOR_UID)),
				   PClassInfo::kManyInstances,  // cardinality
				   kVstAudioEffectClass,        // the component category (do not changed this)
				   PLUG_NAME,                   // here the Plug-in name (to be changed)
				   Vst::kDistributable,         // means component/controller can on different computers
				   VST3_SUBCATEGORY,            // Subcategory for this Plug-in (to be changed)
				   PLUG_VERSION_STR,            // Plug-in version (to be changed)
				   kVstVersionString,           // the VST 3 SDK version (don't change - use define)
				   createProcessorInstance)     // function pointer called to be instantiate

		DEF_CLASS2(INLINE_UID_FROM_FUID(FUID(VST3_CONTROLLER_UID)),
				   PClassInfo::kManyInstances,    // cardinality
				   kVstComponentControllerClass,  // the Controller category (do not changed this)
				   PLUG_NAME " Controller",       // controller name (could be the same than component name)
				   0,                             // not used here
				   "",                            // not used here
				   PLUG_VERSION_STR,              // Plug-in version (to be changed)
				   kVstVersionString,             // the VST 3 SDK version (don't change - use define)
				   createControllerInstance)      // function pointer called to be instantiate

		END_FACTORY
	#endif

#elif defined AU_API
	extern "C"
	{
		#ifndef AU_NO_COMPONENT_ENTRY
		//Component Manager
		IPLUG_EXPORT ComponentResult AUV2_ENTRY(ComponentParameters* pParams, void* pPlug)
		{
			return IPlugAU::IPlugAUEntry(pParams, pPlug);
		}
		#endif

		//>10.7 SDK AUPlugin
		IPLUG_EXPORT void* AUV2_FACTORY(const AudioComponentDescription* pInDesc)
		{
			return IPlugAUFactory<PLUG_CLASS_NAME, PLUG_DOES_MIDI_IN>::Factory(pInDesc);
		}
	};

#elif defined WAM_API
	extern "C"
	{
		EMSCRIPTEN_KEEPALIVE void* createModule()
		{
			Processor* pWAM = dynamic_cast<Processor*>(iplug::MakePlug(InstanceInfo()));
			return (void*) pWAM;
		}
	}

#elif defined WEB_API
	std::unique_ptr<iplug::IPlugWeb> gPlug;
	extern void StartMainLoopTimer();

	extern "C"
	{
		EMSCRIPTEN_KEEPALIVE void iplug_syncfs()
		{
			EM_ASM({
				if (Module.syncdone == 1)
				{
					Module.syncdone = 0;
					FS.syncfs(
						false, function(err) {
							assert(!err);
							console.log("Synced to IDBFS...");
							Module.syncdone = 1;
						});
				}
			});
		}

		EMSCRIPTEN_KEEPALIVE void iplug_fsready()
		{
			gPlug = std::unique_ptr<iplug::IPlugWeb>(iplug::MakePlug(InstanceInfo()));
			gPlug->SetHost("www", 0);
			gPlug->OpenWindow(nullptr);
			iplug_syncfs();  // plug in may initialise settings in constructor, write to persistent data after init
		}
	}

	int main()
	{
		//create persistent data file system and synchronise
		EM_ASM(var name = '/' + UTF8ToString($0) + '_data'; FS.mkdir(name); FS.mount(IDBFS, {}, name);

			   Module.syncdone = 0;
			   FS.syncfs(
				   true,
				   function(err) {
					   assert(!err);
					   console.log("Synced from IDBFS...");
					   Module.syncdone = 1;
					   ccall('iplug_fsready', 'v');
				   });
			   , PLUG_NAME);

		StartMainLoopTimer();

		// TODO: this code never runs, so when do we delete?!
		gPlug = nullptr;

		return 0;
	}

#elif defined AUv3_API || defined AAX_API || defined APP_API
	// Nothing to do here
#else
	#error "No API defined!"
#endif


BEGIN_IPLUG_NAMESPACE

#if AU_API
	Plugin* MakePlug(void* pMemory)
	{
		InstanceInfo info;
		info.mCocoaViewFactoryClassName.Set(AUV2_VIEW_CLASS_STR);

		if (pMemory)
			return new (pMemory) PLUG_CLASS_NAME(info);
		else
			return new PLUG_CLASS_NAME(info);
	}
#elif VST3C_API
	Steinberg::FUnknown* MakeController()
	{
		static WDL_Mutex sMutex;
		WDL_MutexLock lock(&sMutex);
		IPlugVST3Controller::InstanceInfo info;
		info.mOtherGUID = Steinberg::FUID(VST3_PROCESSOR_UID);
		//If you are trying to build a distributed VST3 plug-in and you hit an error here "no matching constructor...",
		//you need to replace all instances of PLUG_CLASS_NAME in your plug-in class, with the macro PLUG_CLASS_NAME
		return static_cast<Steinberg::Vst::IEditController*>(new PLUG_CLASS_NAME(info));
	}
#elif VST3P_API
	Steinberg::FUnknown* MakeProcessor()
	{
		static WDL_Mutex sMutex;
		WDL_MutexLock lock(&sMutex);
		IPlugVST3Processor::InstanceInfo info;
		info.mOtherGUID = Steinberg::FUID(VST3_CONTROLLER_UID);
		return static_cast<Steinberg::Vst::IAudioProcessor*>(new PLUG_CLASS_NAME(info));
	}
#elif VST2_API || VST3_API || AAX_API || AUv3_API || APP_API || WAM_API || WEB_API
	Plugin* MakePlug(const InstanceInfo& info)
	{
		// From VST3 - is this necessary?
		//static WDL_Mutex sMutex;
		//WDL_MutexLock lock(&sMutex);

		return new PLUG_CLASS_NAME(info);
	}

#else
	#error "No API defined!"
#endif

// clang-format on


#pragma mark - ** Config Utility **

END_IPLUG_NAMESPACE


#if !NO_IGRAPHICS
	#include "IGraphics_include_in_plug_src.h"
#endif
