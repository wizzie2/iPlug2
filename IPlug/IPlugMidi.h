/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file
 * @brief MIDI and sysex structs/utilites
 * @ingroup IPlugStructs
 */


BEGIN_IPLUG_NAMESPACE

/** /todo */
enum class EMidiStatusMsg : uint8
{
	None                 = 0x0,
	NoteOff              = 0x8,
	NoteOn               = 0x9,
	PolyphonicAftertouch = 0xA,
	ControlChange        = 0xB,
	ProgramChange        = 0xC,
	ChannelAftertouch    = 0xD,
	PitchBendChange      = 0xE,
	// SystemExclusive      = 0xF0,  // System Common
	// MIDITimeCodeQtrFrame = 0xF1,
	// SongPositionPointer  = 0xF2,
	// SongSelect           = 0xF3,
	// Reserved244          = 0xF4,
	// Reserved245          = 0xF5,
	// TuneRequest          = 0xF6,
	// EndOfSysEx           = 0xF7,
	// TimingClock          = 0xF8,  // System Realtime
	// Reserved249          = 0xF9,
	// Start                = 0xFA,
	// Continue             = 0xFB,
	// Stop                 = 0xFC,
	// Reserved253          = 0xFD,
	// ActiveSensing        = 0xFE,
	// SystemReset          = 0xFF

};


/** /todo */
enum class EMidiControlChangeMsg : uint8
{
	NoCC                            = 0xFF,  //! | --- |
	BankSelect                      = 0,     //! | MSB |
	ModulationWheel                 = 1,     //! | MSB |
	BreathController                = 2,     //! | MSB |
	Undefined03                     = 3,     //! | MSB |
	FootController                  = 4,     //! | MSB |
	PortamentoTime                  = 5,     //! | MSB |
	DataEntry                       = 6,     //! | MSB |
	ChannelVolume                   = 7,     //! | MSB |
	Balance                         = 8,     //! | MSB |
	Undefined09                     = 9,     //! | MSB |
	Pan                             = 10,    //! | MSB |
	ExpressionController            = 11,    //! | MSB |
	EffectControl1                  = 12,    //! | MSB |
	EffectControl2                  = 13,    //! | MSB |
	Undefined014                    = 14,    //! | MSB |
	Undefined015                    = 15,    //! | MSB |
	GeneralPurposeController1       = 16,    //! | MSB |
	GeneralPurposeController2       = 17,    //! | MSB |
	GeneralPurposeController3       = 18,    //! | MSB |
	GeneralPurposeController4       = 19,    //! | MSB |
	Undefined20                     = 20,    //! | MSB |
	Undefined21                     = 21,    //! | MSB |
	Undefined22                     = 22,    //! | MSB |
	Undefined23                     = 23,    //! | MSB |
	Undefined24                     = 24,    //! | MSB |
	Undefined25                     = 25,    //! | MSB |
	Undefined26                     = 26,    //! | MSB |
	Undefined27                     = 27,    //! | MSB |
	Undefined28                     = 28,    //! | MSB |
	Undefined29                     = 29,    //! | MSB |
	Undefined30                     = 30,    //! | MSB |
	Undefined31                     = 31,    //! | MSB |
	LSBBankSelect                   = 32,    //! | LSB |
	LSBModulationWheel              = 33,    //! | LSB |
	LSBBreathController             = 34,    //! | LSB |
	LSBUndefined                    = 35,    //! | LSB |
	LSBFootController               = 36,    //! | LSB |
	LSBPortamentoTime               = 37,    //! | LSB |
	LSBDataEntry                    = 38,    //! | LSB |
	LSBChannelVolume                = 39,    //! | LSB |
	LSBBalance                      = 40,    //! | LSB |
	LSBUndefined09                  = 41,    //! | LSB |
	LSBPan                          = 42,    //! | LSB |
	LSBExpressionController         = 43,    //! | LSB |
	LSBEffectcontrol1               = 44,    //! | LSB |
	LSBEffectcontrol2               = 45,    //! | LSB |
	LSBUndefined14                  = 46,    //! | LSB |
	LSBUndefined15                  = 47,    //! | LSB |
	LSBGeneralPurposeController1    = 48,    //! | LSB |
	LSBGeneralPurposeController2    = 49,    //! | LSB |
	LSBGeneralPurposeController3    = 50,    //! | LSB |
	LSBGeneralPurposeController4    = 51,    //! | LSB |
	LSBUndefined20                  = 52,    //! | LSB |
	LSBUndefined21                  = 53,    //! | LSB |
	LSBUndefined22                  = 54,    //! | LSB |
	LSBUndefined23                  = 55,    //! | LSB |
	LSBUndefined24                  = 56,    //! | LSB |
	LSBUndefined25                  = 57,    //! | LSB |
	LSBUndefined26                  = 58,    //! | LSB |
	LSBUndefined27                  = 59,    //! | LSB |
	LSBUndefined28                  = 60,    //! | LSB |
	LSBUndefined29                  = 61,    //! | LSB |
	LSBUndefined30                  = 62,    //! | LSB |
	LSBUndefined31                  = 63,    //! | LSB |
	DamperPedalState                = 64,    //! | --- | ≤63 off, ≥64 on
	PortamentoState                 = 65,    //! | --- | ≤63 off, ≥64 on
	SostenutoState                  = 66,    //! | --- | ≤63 off, ≥64 on
	SoftPedalState                  = 67,    //! | --- | ≤63 off, ≥64 on
	LegatoState                     = 68,    //! | --- | ≤63 Normal, ≥64 Legato
	Hold2State                      = 69,    //! | --- | ≤63 off, ≥64 on
	SoundControllerSoundVariation   = 70,    //! | LSB | Sound Variation
	SoundControllerTimbre           = 71,    //! | LSB | Resonance
	SoundControllerReleaseTime      = 72,    //! | LSB | Release Time
	SoundControllerAttackTime       = 73,    //! | LSB | Attack Time
	SoundControllerBrightness       = 74,    //! | LSB | Cutoff Frequency
	SoundControllerDecayTime        = 75,    //! | LSB | Decay Time
	SoundControllerVibratoRate      = 76,    //! | LSB | Vibrato Rate
	SoundControllerVibratoDepth     = 77,    //! | LSB | Vibrato Depth
	SoundControllerVibratoDelay     = 78,    //! | LSB | Vibrato Delay
	SoundControllerUndefined10      = 79,    //! | LSB |
	GeneralPurposeController5       = 80,    //! | LSB |
	GeneralPurposeController6       = 81,    //! | LSB |
	GeneralPurposeController7       = 82,    //! | LSB |
	GeneralPurposeController8       = 83,    //! | LSB |
	PortamentoControl               = 84,    //! | LSB |
	Undefined85                     = 85,    //! | --- |
	Undefined86                     = 86,    //! | --- |
	Undefined87                     = 87,    //! | --- |
	HighResolutionVelocityPrefix    = 88,    //! | LSB |
	Undefined89                     = 89,    //! | --- |
	Undefined90                     = 90,    //! | --- |
	Effects1Depth                   = 91,    //! | --- | External Effects Depth
	Effects2Depth                   = 92,    //! | --- | Tremolo Depth
	Effects3Depth                   = 93,    //! | --- | Chorus Depth
	Effects4Depth                   = 94,    //! | --- | Celeste [Detune] Depth
	Effects5Depth                   = 95,    //! | --- | Phaser Depth
	DataIncrement                   = 96,    //! | --- | Data Entry +1
	DataDecrement                   = 97,    //! | --- | Data Entry -1
	NonRegisteredParameterNumberLSB = 98,    //! | LSB | NRPN LSB
	NonRegisteredParameterNumberMSB = 99,    //! | MSB | NRPN MSB
	RegisteredParameterNumberLSB    = 100,   //! | LSB | RPN LSB
	RegisteredParameterNumberMSB    = 101,   //! | MSB | RPN MSB
	Undefined102                    = 102,   //! | --- |
	Undefined103                    = 103,   //! | --- |
	Undefined104                    = 104,   //! | --- |
	Undefined105                    = 105,   //! | --- |
	Undefined106                    = 106,   //! | --- |
	Undefined107                    = 107,   //! | --- |
	Undefined108                    = 108,   //! | --- |
	Undefined109                    = 109,   //! | --- |
	Undefined110                    = 110,   //! | --- |
	Undefined111                    = 111,   //! | --- |
	Undefined112                    = 112,   //! | --- |
	Undefined113                    = 113,   //! | --- |
	Undefined114                    = 114,   //! | --- |
	Undefined115                    = 115,   //! | --- |
	Undefined116                    = 116,   //! | --- |
	Undefined117                    = 117,   //! | --- |
	Undefined118                    = 118,   //! | --- |
	Undefined119                    = 119,   //! | --- |
	AllSoundOff                     = 120,   //! | --- | All Sound Off
	ResetAllControlers              = 121,   //! | --- | Reset All Controllers
	LocalOnOff                      = 122,   //! | --- | Local Control 0 off, 127 on
	AllNotesOff                     = 123,   //! | --- | All Notes Off
	OmniModeOff                     = 124,   //! | --- | Omni Mode Off (+ all notes off)
	OmniModeOn                      = 125,   //! | --- | Omni Mode On (+ all notes off)
	MonoMode                        = 126,   //! | --- | Mono Mode On (+ poly off, + all notes off)
	PolyMode                        = 127    //! | --- | Poly Mode On (+ mono off, +all notes off)
};


/** Encapsulates a MIDI message and provides helper functions
 * @ingroup IPlugStructs */
struct IMidiMsg
{
	int32 mOffset;
	uint8 mData1;	// LSB
	uint8 mData2;	// MSB
	uint8 mStatus;


	/** /todo
	 * @param offs /todo
	 * @param s /todo
	 * @param d1 /todo
	 * @param d2 /todo */
	IMidiMsg(int32 offset = 0, uint8 status = 0, uint8 LSB = 0, uint8 MSB = 0)
		: mOffset(offset)
		, mData1(LSB)
		, mData2(MSB)
		, mStatus(status)
	{
	}

	/** /todo
	 * @param noteNumber /todo
	 * @param velocity /todo
	 * @param offset /todo
	 * @param channel /todo */
	constexpr void SetNoteOn(int noteNumber, float velocity, int offset, int channel = 0)
	{
		DEBUG_ASSERT(math::ClampEval(noteNumber, 0, 127));
		DEBUG_ASSERT(math::ClampEval(velocity, 0, 1));
		DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
		mStatus = static_cast<uint8>(channel | EMidiStatusMsg::NoteOn << 4);
		mData1  = static_cast<uint8>(noteNumber);
		mData2  = static_cast<uint8>(velocity * 127);
		mOffset = offset;
	}

	/** /todo
	 * @param noteNumber /todo
	 * @param offset /todo
	 * @param channel /todo */
	constexpr void SetNoteOff(int noteNumber, int offset, int channel = 0)
	{
		DEBUG_ASSERT(math::ClampEval(noteNumber, 0, 127));
		DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
		mStatus = static_cast<uint8>(channel | EMidiStatusMsg::NoteOff << 4);
		mData1  = static_cast<uint8>(noteNumber);
		mData2  = 0;
		mOffset = offset;
	}

	/** /todo
	 * @param value range [-1, 1], converts to [0, 16383) where 8192 = no pitch change.
	 * @param channel /todo
	 * @param offset /todo */
	constexpr void SetPitchWheel(float pitch, int channel = 0, int offset = 0)
	{
		DEBUG_ASSERT(math::ClampEval(pitch, -1, 1));
		DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
		uint16 i = static_cast<uint16>(pitch > 0.0f ? math::ReMap(pitch, 0.0f, 1.0f, 8193.0f, 16383.0f)
													: math::ReMap(pitch, -1.0f, 0.0f, 0.0f, 8192.0f));
		mStatus  = static_cast<uint8>(channel | EMidiStatusMsg::PitchBendChange << 4);
		mData1   = static_cast<uint8>(i & 0x7F);
		mData2   = static_cast<uint8>(i >> 7);
		mOffset  = offset;
	}

	/** /todo
	 * @param idx /todo
	 * @param value range [0, 1] /todo
	 * @param channel /todo
	 * @param offset /todo */
	constexpr void SetControlChange(EMidiControlChangeMsg idx, float value, int channel = 0, int offset = 0)
	{
		DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
		DEBUG_ASSERT(math::ClampEval(value, 0, 1));
		mStatus = static_cast<uint8>(channel | EMidiStatusMsg::ControlChange << 4);
		mData1  = static_cast<uint8>(idx);
		mData2  = static_cast<uint8>(value * 127.0);
		mOffset = offset;
	}

	/** /todo */
	constexpr void SetProgramChange(int program, int channel = 0, int offset = 0)
	{
		DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
		DEBUG_ASSERT(math::ClampEval(program, 0, 127));
		mStatus = static_cast<uint8>(channel | EMidiStatusMsg::ProgramChange << 4);
		mData1  = static_cast<uint8>(program);
		mData2  = 0;
		mOffset = offset;
	}

	/** /todo
	 * @param pressure /todo
	 * @param offset /todo
	 * @param channel /todo */
	constexpr void SetChannelAftertouch(float pressure, int offset = 0, int channel = 0)
	{
		DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
		DEBUG_ASSERT(math::ClampEval(pressure, 0, 1));
		mStatus = static_cast<uint8>(channel | EMidiStatusMsg::ChannelAftertouch << 4);
		mData1  = static_cast<uint8>(pressure * 127);
		mData2  = 0;
		mOffset = offset;
	}

	/** /todo
	 * @param noteNumber /todo
	 * @param pressure /todo
	 * @param offset /todo
	 * @param channel /todo */
	constexpr void SetPolyphonicAftertouch(int noteNumber, float pressure, int offset, int channel)
	{
		DEBUG_ASSERT(math::ClampEval(noteNumber, 0, 127));
		DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
		DEBUG_ASSERT(math::ClampEval(pressure, 0, 1));
		mStatus = static_cast<uint8>(channel | EMidiStatusMsg::PolyphonicAftertouch << 4);
		mData1  = static_cast<uint8>(noteNumber);
		mData2  = static_cast<uint8>(pressure * 127);
		mOffset = offset;
	}

	/** @return [0, 15] for midi channels 1 ... 16 */
	constexpr int GetChannel() const
	{
		return mStatus & 0x0F;
	}

	/** /todo
	 * @return EMidiStatusMsg /todo */
	constexpr auto GetStatus() const
	{
		EMidiStatusMsg currentStatus = static_cast<EMidiStatusMsg>(mStatus >> 4);
		switch (currentStatus)
		{
			case EMidiStatusMsg::NoteOff:
			case EMidiStatusMsg::NoteOn:
			case EMidiStatusMsg::PolyphonicAftertouch:
			case EMidiStatusMsg::ControlChange:
			case EMidiStatusMsg::ProgramChange:
			case EMidiStatusMsg::ChannelAftertouch:
			case EMidiStatusMsg::PitchBendChange:
				return currentStatus;
			default:
				return EMidiStatusMsg::None;
		}
	}

	/** @return [0, 127), -1 if NA. */
	const int GetNoteNumber() const
	{
		switch (GetStatus())
		{
			case EMidiStatusMsg::NoteOn:
			case EMidiStatusMsg::NoteOff:
			case EMidiStatusMsg::PolyphonicAftertouch:
				return mData1;
			default:
				return -1;
		}
	}

	/** @return returns [0, 1), -1 if NA. */
	constexpr float GetVelocity() const
	{
		switch (GetStatus())
		{
			case EMidiStatusMsg::NoteOn:
			case EMidiStatusMsg::NoteOff:
				return mData2 / 127.f;
			default:
				return -1;
		}
	}

	/** @return [0, 1), -1 if NA. */
	constexpr float GetPolyphonicAftertouch() const
	{
		if (GetStatus() == EMidiStatusMsg::PolyphonicAftertouch)
			return mData2 / 127.f;
		return -1;
	}

	/** @return [0, 1), -1 if NA. */
	constexpr float GetChannelAftertouch() const
	{
		if (GetStatus() == EMidiStatusMsg::ChannelAftertouch)
			return mData1 / 127.f;
		return -1;
	}

	/** @return [0, 127), -1 if NA. */
	constexpr int GetProgram() const
	{
		if (GetStatus() == EMidiStatusMsg::ProgramChange)
			return mData1;
		return -1;
	}

	/** @return [-1.0, 1.0], zero if NA.*/
	constexpr float GetPitchWheel() const
	{
		if (GetStatus() == EMidiStatusMsg::PitchBendChange)
		{
			const float value = static_cast<float>(mData2 << 7 | mData1);
			return value > 8192 ? math::ReMap(value, 8193.0f, 16383.0f, 0.0f, 1.0f)
								: math::ReMap(value, 0.0f, 8192.0f, -1.0f, 0.0f);
		}
		return 0.0f;
	}

	/** /todo
	 * @return EMidiControlChangeMsg /todo */
	constexpr EMidiControlChangeMsg GetControlChangeIdx() const
	{
		return static_cast<EMidiControlChangeMsg>(mData1);
	}

	/** @return [0, 1], -1 if NA.*/
	constexpr float GetControlChange(EMidiControlChangeMsg idx) const
	{
		if (GetStatus() == EMidiStatusMsg::ControlChange && GetControlChangeIdx() == idx)
			return static_cast<float>(mData2) / 127.0f;
		return -1.0f;
	}


	/** /todo
	 * @param msg /todo
	 * @return const char* /todo */
	static constexpr const char* GetStatusStr(const EMidiStatusMsg& msg)
	{
		switch (msg)
		{  // clang-format off
			case EMidiStatusMsg::None:                 return "None";
			case EMidiStatusMsg::NoteOff:              return "NoteOff";
			case EMidiStatusMsg::NoteOn:               return "NoteOn";
			case EMidiStatusMsg::PolyphonicAftertouch: return "Aftertouch";
			case EMidiStatusMsg::ControlChange:        return "ControlChange";
			case EMidiStatusMsg::ProgramChange:        return "ProgramChange";
			case EMidiStatusMsg::ChannelAftertouch:    return "ChannelAftertouch";
			case EMidiStatusMsg::PitchBendChange:      return "PitchbendChange";
			//case EMidiStatusMsg::SystemExclusive:      return "SystemExclusive";
			//case EMidiStatusMsg::MIDITimeCodeQtrFrame: return "MIDITimeCodeQtrFrame";
			//case EMidiStatusMsg::SongPositionPointer:  return "SongPositionPointer";
			//case EMidiStatusMsg::SongSelect:           return "SongSelect";
			//case EMidiStatusMsg::Reserved244:          return "Reserved244";
			//case EMidiStatusMsg::Reserved245:          return "Reserved245";
			//case EMidiStatusMsg::TuneRequest:          return "TuneRequest";
			//case EMidiStatusMsg::EndOfSysEx:           return "EndOfSysEx";
			//case EMidiStatusMsg::TimingClock:          return "TimingClock";
			//case EMidiStatusMsg::Reserved249:          return "Reserved249";
			//case EMidiStatusMsg::Start:                return "Start";
			//case EMidiStatusMsg::Continue:             return "Continue";
			//case EMidiStatusMsg::Stop:                 return "Stop";
			//case EMidiStatusMsg::Reserved253:          return "Reserved253";
			//case EMidiStatusMsg::ActiveSensing:        return "ActiveSensing";
			//case EMidiStatusMsg::SystemReset:          return "SystemReset";
			default:                                   return "unknown";
		};  // clang-format on
	}

	static constexpr const char* GetCCNameStr(int idx)
	{
		constexpr const char* ccNameStrs[128] = {
			"BankSel.MSB", "Modulation",  "BreathCtrl",   "Contr. 3",    "Foot Ctrl",   "Porta.Time",  "DataEntMSB",
			"MainVolume",  "Balance",     "Contr. 9",     "Pan",         "Expression",  "FXControl1",  "FXControl2",
			"Contr. 14",   "Contr. 15",   "Gen.Purp.1",   "Gen.Purp.2",  "Gen.Purp.3",  "Gen.Purp.4",  "Contr. 20",
			"Contr. 21",   "Contr. 22",   "Contr. 23",    "Contr. 24",   "Contr. 25",   "Contr. 26",   "Contr. 27",
			"Contr. 28",   "Contr. 29",   "Contr. 30",    "Contr. 31",   "BankSel.LSB", "Modul. LSB",  "BrthCt LSB",
			"Contr. 35",   "FootCt LSB",  "Port.T LSB",   "DataEntLSB",  "MainVolLSB",  "BalanceLSB",  "Contr. 41",
			"Pan LSB",     "Expr. LSB",   "Contr. 44",    "Contr. 45",   "Contr. 46",   "Contr. 47",   "Gen.P.1LSB",
			"Gen.P.2LSB",  "Gen.P.3LSB",  "Gen.P.4LSB",   "Contr. 52",   "Contr. 53",   "Contr. 54",   "Contr. 55",
			"Contr. 56",   "Contr. 57",   "Contr. 58",    "Contr. 59",   "Contr. 60",   "Contr. 61",   "Contr. 62",
			"Contr. 63",   "Damper Ped",  "Porta. Ped",   "Sostenuto ",  "Soft Pedal",  "Legato Sw",   "Hold 2",
			"SoundCont 1", "SoundCont 2", "SoundCont 3",  "SoundCont 4", "SoundCont 5", "SoundCont 6", "SoundCont 7",
			"SoundCont 8", "SoundCont 9", "SoundCont 10", "Gen.Purp.5",  "Gen.Purp.6",  "Gen.Purp.7",  "Gen.Purp.8",
			"Portamento",  "Contr. 85",   "Contr. 86",    "Contr. 87",   "Contr. 88",   "Contr. 89",   "Contr. 90",
			"FX 1 Depth",  "FX 2 Depth",  "FX 3 Depth",   "FX 4 Depth",  "FX 5 Depth",  "Data Incr",   "Data Decr",
			"Non-RegLSB",  "Non-RegMSB",  "Reg LSB",      "Reg MSB",     "Contr. 102",  "Contr. 103",  "Contr. 104",
			"Contr. 105",  "Contr. 106",  "Contr. 107",   "Contr. 108",  "Contr. 109",  "Contr. 110",  "Contr. 111",
			"Contr. 112",  "Contr. 113",  "Contr. 114",   "Contr. 115",  "Contr. 116",  "Contr. 117",  "Contr. 118",
			"Contr. 119",  "Contr. 120",  "Reset Ctrl",   "Local Ctrl",  "AllNoteOff",  "OmniModOff",  "OmniModeOn",
			"MonoModeOn",  "PolyModeOn"};

		return ccNameStrs[idx];
	}
	/** /todo */
	inline const void LogMsg()
	{
		Trace(TRACELOC, "midi:(%s:%d:%d:%d)", GetStatusStr(GetStatus()), GetChannel(), mData1, mData2);
	}

	/** /todo */
	inline const void PrintMsg() const
	{
		// Not using any non-static IMidiMsg functions in case of recursive calling
		DBGMSG("midi: offset %i, (%s:%d:%d:%d)\n",
			   mOffset,
			   IMidiMsg::GetStatusStr(static_cast<EMidiStatusMsg>(mStatus >> 4)),
			   mStatus & 0x0F,
			   mData1,
			   mData2);
	}
};

/** A struct for dealing with SysEx messages. Does not own the data.
 * @ingroup IPlugStructs */
struct ISysEx
{
	int mOffset;
	int mSize;
	const uint8_t* mData;

	/** /todo
	 * @param offs /todo
	 * @param pData /todo
	 * @param size /todo */
	ISysEx(int offs = 0, const uint8_t* pData = nullptr, int size = 0) : mOffset(offs), mSize(size), mData(pData) {}

	/** /todo */
	void Clear()
	{
		mOffset = mSize = 0;
		mData           = NULL;
	}

	/** /todo
	 * @param str /todo
	 * @param maxlen /todo
	 * @param pData /todo
	 * @param size /todo
	 * @return char* /todo */
	char* SysExStr(char* str, int maxlen, const uint8_t* pData, int size)
	{
		assert(str != NULL && maxlen >= 3);

		if (!pData || !size)
		{
			*str = '\0';
			return str;
		}

		char* pStr = str;
		int n      = maxlen / 3;
		if (n > size)
			n = size;
		for (int i = 0; i < n; ++i, ++pData)
		{
			sprintf(pStr, "%02X", (int) *pData);
			pStr += 2;
			*pStr++ = ' ';
		}
		*--pStr = '\0';

		return str;
	}

	void LogMsg()
	{
		char str[96];
		Trace(TRACELOC, "sysex:(%d:%s)", mSize, SysExStr(str, sizeof(str), mData, mSize));
	}
};

/*

IMidiQueue
(c) Theo Niessink 2009-2011
<http://www.taletn.com/>


This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software in a
   product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.


IMidiQueue is a fast, lean & mean MIDI queue for IPlug instruments or
effects. Here are a few code snippets showing how to implement IMidiQueue in
an IPlug project:


MyPlug.h:

#include "WDL/IPlug/IMidiQueue.h"

class MyPlug: public IPlug
{
protected:
  IMidiQueue mMidiQueue;
}


MyPlug.cpp:

void MyPlug::OnReset()
{
  mMidiQueue.Resize(GetBlockSize());
}

void MyPlug::ProcessMidiMsg(IMidiMsg* pMsg)
{
  mMidiQueue.Add(pMsg);
}

void MyPlug::ProcessBlock(double** inputs, double** outputs, int nFrames)
{
  for (int offset = 0; offset < nFrames; ++offset)
  {
	while (!mMidiQueue.Empty())
	{
	  IMidiMsg* pMsg = mMidiQueue.Peek();
	  if (msg.mOffset > offset) break;

	  // To-do: Handle the MIDI message

	  mMidiQueue.Remove();
	}

	// To-do: Process audio

  }
  mMidiQueue.Flush(nFrames);
}

*/


/** A class to help with queuing timestamped MIDI messages
 * @ingroup IPlugUtilities */
class IMidiQueue
{
 public:
	IMidiQueue(int size = Config::defaultMidiQueueBlockSize)
		: mBuf(nullptr)
		, mSize(0)
		, mGrow(Granulize(size))
		, mFront(0)
		, mBack(0)
	{
		Expand();
	}

	~IMidiQueue()
	{
		free(mBuf);  // TODO: implement real memory manager
	}

	// Adds a MIDI message at the back of the queue. If the queue is full,
	// it will automatically expand itself.
	void Add(const IMidiMsg& msg)
	{
		if (mBack >= mSize)
		{
			if (mFront > 0)
				Compact();
			else if (!Expand())
				return;
		}
		auto index = mBack;
		if constexpr (Config::Midi::SortIMidiQueue)
		{
			// Insert the MIDI message at the right offset.
			if (mBack > mFront && msg.mOffset < mBuf[mBack - 1].mOffset)
			{
				index -= 2;
				while (index >= mFront && msg.mOffset < mBuf[index].mOffset)
					--index;
				index++;
				memmove(&mBuf[index + 1], &mBuf[index], (mBack - index) * sizeof(IMidiMsg));
			}
		}
		mBuf[index] = msg;
		++mBack;

		// DBGMSG("IMidiQueue.Add: offset %i, (%s:%d:%d:%d)\n",
		//	   msg.mOffset,
		//	   msg.GetStatusStr(msg.GetStatus()),
		//	   msg.GetChannel(),
		//	   msg.mData1,
		//	   msg.mData2);
	}

	// Removes a MIDI message from the front of the queue (but does *not*
	// free up its space until Compact() is called).
	inline void Remove()
	{
		++mFront;
	}

	// Returns true if the queue is empty.
	inline bool Empty() const
	{
		return mFront == mBack;
	}

	// Returns the number of MIDI messages in the queue.
	inline int ToDo() const
	{
		return mBack - mFront;
	}

	// Returns the number of MIDI messages for which memory has already been
	// allocated.
	inline int GetSize() const
	{
		return mSize;
	}

	// Returns the "next" MIDI message (all the way in the front of the
	// queue), but does *not* remove it from the queue.
	inline IMidiMsg& Peek() const
	{
		return mBuf[mFront];
	}

	// Moves back MIDI messages all the way to the front of the queue, thus
	// freeing up space at the back, and updates the sample offset of the
	// remaining MIDI messages by substracting nFrames.
	inline void Flush(int nFrames)
	{
		// Move everything all the way to the front.
		if (mFront > 0)
			Compact();

		// Update the sample offset.
		for (int i = 0; i < mBack; ++i)
			mBuf[i].mOffset -= nFrames;
	}

	// Clears the queue.
	inline void Clear()
	{
		mFront = mBack = 0;
	}

	// Resizes (grows or shrinks) the queue, returns the new size.
	int Resize(int size)
	{
		if (mFront > 0)
			Compact();
		mGrow = size = Granulize(size);
		// Don't shrink below the number of currently queued MIDI messages.
		if (size < mBack)
			size = Granulize(mBack);
		if (size == mSize)
			return mSize;

		void* buf = realloc(mBuf, size * sizeof(IMidiMsg));
		if (!buf)
			return mSize;

		mBuf  = (IMidiMsg*) buf;
		mSize = size;
		DBGMSG("IMidiQueue.Resize(%i): new size = %i.", size, mSize);
		return size;
	}

 private:
	// Automatically expands the queue.
	bool Expand()
	{
		if (!mGrow)
			return false;
		int size = (mSize / mGrow + 1) * mGrow;

		void* buf = realloc(mBuf, size * sizeof(IMidiMsg));
		if (!buf)
			return false;

		mBuf  = (IMidiMsg*) buf;
		mSize = size;
		DBGMSG("IMidiQueue.Expand(): new size = %i.", size, mSize);
		return true;
	}

	// Moves everything all the way to the front.
	inline void Compact()
	{
		mBack -= mFront;
		if (mBack > 0)
			memmove(&mBuf[0], &mBuf[mFront], mBack * sizeof(IMidiMsg));
		mFront = 0;
	}

	// Rounds the MIDI queue size up to the next 4 kB memory page size.
	inline int Granulize(int size) const
	{
		size_t bytes = size * sizeof(IMidiMsg);
		size_t rest  = bytes % 4096;
		if (rest)
			size = (bytes - rest + 4096) / sizeof(IMidiMsg);
		return size;
	}

	IMidiMsg* mBuf = nullptr;
	size_t mSize, mGrow;
	size_t mFront, mBack;
};

END_IPLUG_NAMESPACE
