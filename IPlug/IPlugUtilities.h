/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file
 * Utility functions and macros
 * @defgroup IPlugUtilities IPlug::Utilities
 * Utility functions and macros
 * @{
 */


BEGIN_IPLUG_NAMESPACE

template <typename T>
DEPRECATED(2.0, "Deprecated. Use math::Clamp instead")
inline constexpr T Clip(T x, T lo, T hi)
{
	return math::Clamp<T>(x, lo, hi);
}

/** Linear interpolate between values \p a and \p b
 * @param a Low value
 * @param b High value
 * @param f Value betweeen 0-1 for interpolation */
template <typename T>
inline T Lerp(T a, T b, T f)
{
	return ((b - a) * f + a);
}

static inline bool CStringHasContents(const char* str)
{
	return str && str[0] != '\0';
}

#define MAKE_QUOTE(str) #str
#define MAKE_STR(str)   MAKE_QUOTE(str)

/** @hideinitializer */
#define GET_PARAM_FROM_VARARG(paramType, vp, v)  \
	{                                            \
		v = 0.0;                                 \
		switch (paramType)                       \
		{                                        \
			case IParam::kTypeBool:              \
			case IParam::kTypeInt:               \
			case IParam::kTypeEnum:              \
			{                                    \
				v = (double) va_arg(vp, int);    \
				break;                           \
			}                                    \
			case IParam::kTypeDouble:            \
			default:                             \
			{                                    \
				v = (double) va_arg(vp, double); \
				break;                           \
			}                                    \
		}                                        \
	}


/** /todo
 * @param version /todo
 * @param ver /todo
 * @param maj /todo
 * @param min /todo */
static inline void GetVersionParts(int version, int& ver, int& maj, int& min)
{
	ver = (version & 0xFFFF0000) >> 16;
	maj = (version & 0x0000FF00) >> 8;
	min = version & 0x000000FF;
}

/** /todo
 * @param version /todo
 * @return int /todo */
static inline int GetDecimalVersion(int version)
{
	int ver, rmaj, rmin;
	GetVersionParts(version, ver, rmaj, rmin);
	return 10000 * ver + 100 * rmaj + rmin;
}

/** /todo
 * @param version /todo
 * @param str /todo */
static inline void GetVersionStr(int version, WDL_String& str)
{
	int ver, rmaj, rmin;
	GetVersionParts(version, ver, rmaj, rmin);
	str.SetFormatted(MAX_VERSION_STR_LEN, "v%d.%d.%d", ver, rmaj, rmin);
}

/** /todo
 * @tparam SRC
 * @tparam DEST
 * @param pDest /todo
 * @param pSrc /todo
 * @param n /todo */
template <class SRC, class DEST>
void CastCopy(DEST* pDest, SRC* pSrc, int n)
{
	for (int i = 0; i < n; ++i, ++pDest, ++pSrc)
	{
		*pDest = (DEST) *pSrc;
	}
}

/** /todo
 * @param cDest /todo
 * @param cSrc /todo */
static void ToLower(char* cDest, const char* cSrc)
{
	int i;
	int n = static_cast<int>(strlen(cSrc));
	for (i = 0; i < n; ++i)
	{
		cDest[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(cSrc[i])));
	}
	cDest[i] = '\0';
}

// clang-format off
/** Gets the host ID from a human-readable name
 * @param inHost Host name to search for
 * @return Identifier of the host (see ::EHost) */
static EHost LookUpHost(const char* inHost)
{
	char host[256];
	ToLower(host, inHost);
	
	// C4 is version >= 8.2
	if (strstr(host, "reaper"))               return EHost::kHostReaper;
	if (strstr(host, "protools"))             return EHost::kHostProTools;
	if (strstr(host, "cubase"))               return EHost::kHostCubase;
	if (strstr(host, "nuendo"))               return EHost::kHostNuendo;
	if (strstr(host, "cakewalk"))             return EHost::kHostSonar;
	if (strstr(host, "vegas"))                return EHost::kHostVegas;
	if (strstr(host, "fruity"))               return EHost::kHostFL;
	if (strstr(host, "samplitude"))           return EHost::kHostSamplitude;
	if (strstr(host, "live"))                 return EHost::kHostAbletonLive;
	if (strstr(host, "tracktion"))            return EHost::kHostTracktion;
	if (strstr(host, "ntracks"))              return EHost::kHostNTracks;
	if (strstr(host, "melodyne"))             return EHost::kHostMelodyneStudio;
	if (strstr(host, "vstmanlib"))            return EHost::kHostVSTScanner;
	if (strstr(host, "aulab"))                return EHost::kHostAULab;
	if (strstr(host, "forte"))                return EHost::kHostForte;
	if (strstr(host, "chainer"))              return EHost::kHostChainer;
	if (strstr(host, "audition"))             return EHost::kHostAudition;
	if (strstr(host, "orion"))                return EHost::kHostOrion;
	if (strstr(host, "bias"))                 return EHost::kHostBias;
	if (strstr(host, "sawstudio"))            return EHost::kHostSAWStudio;
	if (strstr(host, "logic"))                return EHost::kHostLogic;
	if (strstr(host, "garageband"))           return EHost::kHostGarageBand;
	if (strstr(host, "digital"))              return EHost::kHostDigitalPerformer;
	if (strstr(host, "audiomulch"))           return EHost::kHostAudioMulch;
	if (strstr(host, "presonus"))             return EHost::kHostStudioOne;
	if (strstr(host, "vst3plugintesthost"))   return EHost::kHostVST3TestHost;
	if (strstr(host, "ardour"))               return EHost::kHostArdour;
	if (strstr(host, "renoise"))              return EHost::kHostRenoise;
	if (strstr(host, "openmpt"))              return EHost::kHostOpenMPT;
	if (strstr(host, "wavelab elements"))     return EHost::kHostWaveLabElements; // check for wavelab elements should come before wavelab ...
	if (strstr(host, "wavelab"))              return EHost::kHostWaveLab;
	if (strstr(host, "twistedwave"))          return EHost::kHostTwistedWave;
	if (strstr(host, "bitwig studio"))        return EHost::kHostBitwig;
	if (strstr(host, "reason"))               return EHost::kHostReason;
	if (strstr(host, "gwvst"))                return EHost::kHostGoldWave5x;
	if (strstr(host, "waveform"))             return EHost::kHostWaveform;
	if (strstr(host, "audacity"))             return EHost::kHostAudacity;
	if (strstr(host, "acoustica"))            return EHost::kHostAcoustica;
	if (strstr(host, "plugindoctor"))         return EHost::kHostPluginDoctor;
	if (strstr(host, "izotope rx"))           return EHost::kHostiZotopeRX;
	if (strstr(host, "savihost"))             return EHost::kHostSAVIHost;
	if (strstr(host, "blue cat's vst host"))  return EHost::kHostBlueCat;
	
	if (strstr(host, "standalone"))           return EHost::kHostStandalone;
	if (strstr(host, "www"))                  return EHost::kHostWWW;
	
	return EHost::kHostUnknown;
}

/** Gets a human-readable name from host identifier
 * @param host Host identifier (see ::EHost)
 * @param str WDL_String to set
 * @code
 *    int hostID = EHost::kHostAbletonLive;
 *    WDL_String hostName;
 *    GetHostNameStr(hostID, hostName);
 * @endcode*/
static void GetHostNameStr(EHost host, WDL_String& str)
{
	switch (host)
	{
		case EHost::kHostReaper:             str.Set("reaper");              break;
		case EHost::kHostProTools:           str.Set("protools");            break;
		case EHost::kHostCubase:             str.Set("cubase");              break;
		case EHost::kHostNuendo:             str.Set("nuendo");              break;
		case EHost::kHostSonar:              str.Set("cakewalk");            break;
		case EHost::kHostVegas:              str.Set("vegas");               break;
		case EHost::kHostFL:                 str.Set("fruity");              break;
		case EHost::kHostSamplitude:         str.Set("samplitude");          break;
		case EHost::kHostAbletonLive:        str.Set("live");                break;
		case EHost::kHostTracktion:          str.Set("tracktion");           break;
		case EHost::kHostNTracks:            str.Set("ntracks");             break;
		case EHost::kHostMelodyneStudio:     str.Set("melodyne");            break;
		case EHost::kHostVSTScanner:         str.Set("vstmanlib");           break;
		case EHost::kHostAULab:              str.Set("aulab");               break;
		case EHost::kHostForte:              str.Set("forte");               break;
		case EHost::kHostChainer:            str.Set("chainer");             break;
		case EHost::kHostAudition:           str.Set("audition");            break;
		case EHost::kHostOrion:              str.Set("orion");               break;
		case EHost::kHostBias:               str.Set("bias");                break;
		case EHost::kHostSAWStudio:          str.Set("sawstudio");           break;
		case EHost::kHostLogic:              str.Set("logic");               break;
		case EHost::kHostGarageBand:         str.Set("garageband");          break;
		case EHost::kHostDigitalPerformer:   str.Set("digital");             break;
		case EHost::kHostAudioMulch:         str.Set("audiomulch");          break;
		case EHost::kHostStudioOne:          str.Set("presonus");            break;
		case EHost::kHostVST3TestHost:       str.Set("vst3plugintesthost");  break;
		case EHost::kHostArdour:             str.Set("ardour");              break;
		case EHost::kHostRenoise:            str.Set("renoise");             break;
		case EHost::kHostOpenMPT:            str.Set("OpenMPT");             break;
		case EHost::kHostWaveLabElements:    str.Set("wavelab elements");    break;
		case EHost::kHostWaveLab:            str.Set("wavelab");             break;
		case EHost::kHostTwistedWave:        str.Set("twistedwave");         break;
		case EHost::kHostBitwig:             str.Set("bitwig studio");       break;
		case EHost::kHostReason:             str.Set("reason");              break;
		case EHost::kHostGoldWave5x:         str.Set("gwvst");               break;
		case EHost::kHostWaveform:           str.Set("waveform");            break;
		case EHost::kHostAudacity:           str.Set("audacity");            break;
		case EHost::kHostAcoustica:          str.Set("acoustica");           break;
		case EHost::kHostPluginDoctor:       str.Set("plugindoctor");        break;
		case EHost::kHostiZotopeRX:          str.Set("izotope rx");          break;
		case EHost::kHostSAVIHost:           str.Set("savihost");            break;
		case EHost::kHostBlueCat:            str.Set("blue cat's vst host"); break;
		
		case EHost::kHostStandalone:         str.Set("standalone");          break;
		case EHost::kHostWWW:                str.Set("www");                 break;

		default:
			str.Set("Unknown");
			break;
	}
}
// clang-format on

/** /todo
 * @param midiPitch /todo
 * @param noteName /todo
 * @param cents /todo
 * @param middleCisC4 /todo */
static void MidiNoteName(float midiPitch, WDL_String& noteName, bool cents = false, bool middleCisC4 = false)
{
	static const char noteNames[12][3] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};

	int midiPitchR = math::RoundToInt(midiPitch);
	int pitchClass = midiPitchR % 12;
	int octave     = (midiPitchR / 12) - (middleCisC4 ? 1 : 2);

	if (cents)
	{
		double frac = (midiPitch - (float) midiPitchR) * 100.;
		noteName.SetFormatted(32, "%s%i %.0f", noteNames[pitchClass], octave, frac);
	}
	else
	{
		noteName.SetFormatted(32, "%s%i", noteNames[pitchClass], octave);
	}
}

END_IPLUG_NAMESPACE

/**@}*/
