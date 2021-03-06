/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file
 * @brief IPlug Constant definitions, Types, magic numbers
 * @defgroup IPlugConstants IPlug::Constants
 * IPlug Constant definitions, Types, magic numbers
 * @{
 */

namespace iplug
{
	using PLUG_SAMPLE_DST = std::conditional_t<type::IsDouble<tfloat>, double, float>;
	using PLUG_SAMPLE_SRC = std::conditional_t<type::IsDouble<tfloat>, float, double>;
	using sample          = tfloat;


	inline constexpr int kNoParameter = -1;
	inline constexpr int kNoValIdx    = -1;
	inline constexpr int kNoTag       = -1;

	enum class EPluginType
	{
		Effect,
		Instrument,
		MIDIEffect
	};


	/** @enum EParamSource
	 * Used to identify the source of a parameter change
	 */
	enum class EParamSource
	{
		kReset,
		kHost,
		kPresetRecall,
		kUI,
		kDelegate,
		kRecompile,  // for FAUST JIT
		kUnknown
	};
	static type::EnumStringMap<EParamSource> ParamSourceStrs {{EParamSource::kReset, "Reset"},
															  {EParamSource::kHost, "Host"},
															  {EParamSource::kPresetRecall, "Preset"},
															  {EParamSource::kUI, "UI"},
															  {EParamSource::kDelegate, "Editor Delegate"},
															  {EParamSource::kRecompile, "Recompile"},
															  {EParamSource::kUnknown, "Unknown"}};


	/** @enum ERoute
	 * Used to identify whether a bus/channel connection is an input or an output
	 */
	enum class ERoute
	{
		kInput  = 0,
		kOutput = 1
	};
	static type::EnumStringMap<ERoute> RoutingDirStrs {{ERoute::kInput, "Input"}, {ERoute::kOutput, "Output"}};

	/** @enum EHost
	 * Host identifier
	 */
	enum class EHost
	{
		kHostUninit  = -1,
		kHostUnknown = 0,
		kHostReaper,
		kHostProTools,
		kHostCubase,
		kHostNuendo,
		kHostSonar,
		kHostVegas,
		kHostFL,
		kHostSamplitude,
		kHostAbletonLive,
		kHostTracktion,
		kHostNTracks,
		kHostMelodyneStudio,
		kHostVSTScanner,
		kHostAULab,
		kHostForte,
		kHostChainer,
		kHostAudition,
		kHostOrion,
		kHostBias,
		kHostSAWStudio,
		kHostLogic,
		kHostGarageBand,
		kHostDigitalPerformer,
		kHostStandalone,
		kHostAudioMulch,
		kHostStudioOne,
		kHostVST3TestHost,
		kHostArdour,
		kHostRenoise,
		kHostOpenMPT,
		kHostWaveLab,
		kHostWaveLabElements,
		kHostTwistedWave,
		kHostBitwig,
		kHostWWW,

		kHostReason,
		kHostGoldWave5x,
		kHostWaveform,
		kHostAudacity,
		kHostAcoustica,
		kHostPluginDoctor,
		kHostiZotopeRX,
		kHostSAVIHost,
		kHostBlueCat,

		// These hosts don't report the host name:
		// EnergyXT2
		// MiniHost
	};

	enum EResourceLocation
	{
		kNotFound = 0,
		kAbsolutePath,
		kWinBinary,
		kPreloadedTexture
	};

	// These constants come from vstpreset.cpp, allowing saving of VST3 format presets without including the VST3 SDK
	// typedef char ChunkID[4];
	using ChunkID = char[4];

	enum ChunkType
	{
		kHeader,
		kComponentState,
		kControllerState,
		kProgramData,
		kMetaInfo,
		kChunkList,
		kNumPresetChunks
	};

	static const ChunkID commonChunks[kNumPresetChunks] = {
		{'V', 'S', 'T', '3'},  // kHeader
		{'C', 'o', 'm', 'p'},  // kComponentState
		{'C', 'o', 'n', 't'},  // kControllerState
		{'P', 'r', 'o', 'g'},  // kProgramData
		{'I', 'n', 'f', 'o'},  // kMetaInfo
		{'L', 'i', 's', 't'}   // kChunkList
	};

	// Preset Header: header id + version + class id + list offset
	inline constexpr int32 kFormatVersion = 1;
	inline constexpr int32 kClassIDSize   = 32;  // ASCII-encoded FUID
	inline constexpr int32 kHeaderSize    = sizeof(ChunkID) + sizeof(int32) + kClassIDSize + sizeof(int64);
	// static const int32_t kListOffsetPos = kHeaderSize - sizeof (int64_t);

	// Preset Version Constants
	inline constexpr int kFXPVersionNum = 1;
	inline constexpr int kFXBVersionNum = 2;

	// This enumeration must match win32 Fkeys as specified in winuser.h
	enum ESpecialKey
	{
		kFVIRTKEY = 0x01,
		kFSHIFT   = 0x04,
		kFCONTROL = 0x08,
		kFALT     = 0x10,
		kFLWIN    = 0x20
	};

	// This enumeration must match win32 virtual keys as specified in winuser.h
	enum EVirtualKey
	{
		kVK_NONE      = 0x00,
		kVK_LBUTTON   = 0x01,
		kVK_RBUTTON   = 0x02,
		kVK_MBUTTON   = 0x04,
		kVK_BACK      = 0x08,
		kVK_TAB       = 0x09,
		kVK_CLEAR     = 0x0C,
		kVK_RETURN    = 0x0D,
		kVK_SHIFT     = 0x10,
		kVK_CONTROL   = 0x11,
		kVK_MENU      = 0x12,
		kVK_PAUSE     = 0x13,
		kVK_CAPITAL   = 0x14,
		kVK_ESCAPE    = 0x1B,
		kVK_SPACE     = 0x20,
		kVK_PRIOR     = 0x21,
		kVK_NEXT      = 0x22,
		kVK_END       = 0x23,
		kVK_HOME      = 0x24,
		kVK_LEFT      = 0x25,
		kVK_UP        = 0x26,
		kVK_RIGHT     = 0x27,
		kVK_DOWN      = 0x28,
		kVK_SELECT    = 0x29,
		kVK_PRINT     = 0x2A,
		kVK_SNAPSHOT  = 0x2C,
		kVK_INSERT    = 0x2D,
		kVK_DELETE    = 0x2E,
		kVK_HELP      = 0x2F,
		kVK_0         = 0x30,
		kVK_1         = 0x31,
		kVK_2         = 0x32,
		kVK_3         = 0x33,
		kVK_4         = 0x34,
		kVK_5         = 0x35,
		kVK_6         = 0x36,
		kVK_7         = 0x37,
		kVK_8         = 0x38,
		kVK_9         = 0x39,
		kVK_A         = 0x41,
		kVK_B         = 0x42,
		kVK_C         = 0x43,
		kVK_D         = 0x44,
		kVK_E         = 0x45,
		kVK_F         = 0x46,
		kVK_G         = 0x47,
		kVK_H         = 0x48,
		kVK_I         = 0x49,
		kVK_J         = 0x4A,
		kVK_K         = 0x4B,
		kVK_L         = 0x4C,
		kVK_M         = 0x4D,
		kVK_N         = 0x4E,
		kVK_O         = 0x4F,
		kVK_P         = 0x50,
		kVK_Q         = 0x51,
		kVK_R         = 0x52,
		kVK_S         = 0x53,
		kVK_T         = 0x54,
		kVK_U         = 0x55,
		kVK_V         = 0x56,
		kVK_W         = 0x57,
		kVK_X         = 0x58,
		kVK_Y         = 0x59,
		kVK_Z         = 0x5A,
		kVK_LWIN      = 0x5B,
		kVK_NUMPAD0   = 0x60,
		kVK_NUMPAD1   = 0x61,
		kVK_NUMPAD2   = 0x62,
		kVK_NUMPAD3   = 0x63,
		kVK_NUMPAD4   = 0x64,
		kVK_NUMPAD5   = 0x65,
		kVK_NUMPAD6   = 0x66,
		kVK_NUMPAD7   = 0x67,
		kVK_NUMPAD8   = 0x68,
		kVK_NUMPAD9   = 0x69,
		kVK_MULTIPLY  = 0x6A,
		kVK_ADD       = 0x6B,
		kVK_SEPARATOR = 0x6C,
		kVK_SUBTRACT  = 0x6D,
		kVK_DECIMAL   = 0x6E,
		kVK_DIVIDE    = 0x6F,
		kVK_F1        = 0x70,
		kVK_F2        = 0x71,
		kVK_F3        = 0x72,
		kVK_F4        = 0x73,
		kVK_F5        = 0x74,
		kVK_F6        = 0x75,
		kVK_F7        = 0x76,
		kVK_F8        = 0x77,
		kVK_F9        = 0x78,
		kVK_F10       = 0x79,
		kVK_F11       = 0x7A,
		kVK_F12       = 0x7B,
		kVK_F13       = 0x7C,
		kVK_F14       = 0x7D,
		kVK_F15       = 0x7E,
		kVK_F16       = 0x7F,
		kVK_F17       = 0x80,
		kVK_F18       = 0x81,
		kVK_F19       = 0x82,
		kVK_F20       = 0x83,
		kVK_F21       = 0x84,
		kVK_F22       = 0x85,
		kVK_F23       = 0x86,
		kVK_F24       = 0x87,
		kVK_NUMLOCK   = 0x90,
		kVK_SCROLL    = 0x91
	};


#ifndef IDLE_TIMER_RATE
	#define IDLE_TIMER_RATE 15  // this controls the frequency of data going from processor to editor (and OnIdle calls)
#endif

#ifndef MAX_SYSEX_SIZE
	#define MAX_SYSEX_SIZE 512
#endif


#define MAX_PRESET_NAME_LEN 256

#define LOGFILE                 "IPlugLog.txt"
#define MAX_PROCESS_TRACE_COUNT 100
#define MAX_IDLE_TRACE_COUNT    15

#define UNUSED_PRESET_NAME       "empty"
#define DEFAULT_USER_PRESET_NAME "user preset"

#define AU_MAX_IO_CHANNELS 128

// TODO: check this shit really?
#define MAX_MACOS_PATH_LEN      1024
#define MAX_WIN32_PATH_LEN      256
#define MAX_WIN32_PARAM_LEN     256
#define IPLUG_WIN_MAX_WIDE_PATH 4096

#define MAX_PLUGIN_NAME_LEN 128

#define MAX_PARAM_NAME_LEN    32  // e.g. "Gain"
#define MAX_PARAM_LABEL_LEN   32  // e.g. "Percent"
#define MAX_PARAM_DISPLAY_LEN 32  // e.g. "100" / "Mute"
#define MAX_PARAM_GROUP_LEN   32  // e.g. "oscillator section"
#define MAX_BUS_NAME_LEN      32  // e.g. "sidechain input"
#define MAX_CHAN_NAME_LEN     32  // e.g. "input 1"

#define MAX_VERSION_STR_LEN         32
#define MAX_BUILD_INFO_STR_LEN      256
#define MAX_PARAM_DISPLAY_PRECISION 6

#define MAX_AAX_PARAMID_LEN 32

#define PARAM_UNINIT 99.99e-9

#define PARAM_TRANSFER_SIZE 512
#define MIDI_TRANSFER_SIZE  32
#define SYSEX_TRANSFER_SIZE 4

// All version ints are stored as 0xVVVVRRMM: V = version, R = revision, M = minor revision.
#define IPLUG_VERSION       0x010000
#define IPLUG_VERSION_MAGIC 'pfft'

#define MAX_BUS_CHANS 64  // wild cards in channel i/o strings will result in this many channels


}  // namespace iplug
