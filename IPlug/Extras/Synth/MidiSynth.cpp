/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
 */

#include "MidiSynth.h"

using namespace iplug;

MidiSynth::MidiSynth(VoiceAllocator::EPolyMode mode, int blockSize) : mBlockSize(blockSize)
{
	SetPolyMode(mode);

	// initialize Channel states
	for (int i = 0; i < 16; ++i)
	{
		mChannelStates[i]                = ChannelState {0};
		mChannelStates[i].pitchBendRange = kDefaultPitchBendRange;
	}
}

MidiSynth::~MidiSynth() {}

VoiceInputEvent MidiSynth::MidiMessageToEventBasic(const IMidiMsg& msg)
{
	VoiceInputEvent event {};

	EMidiStatusMsg status   = msg.GetStatus();
	event.mSampleOffset     = msg.mOffset;
	event.mAddress.mChannel = msg.GetChannel();
	event.mAddress.mKey     = msg.GetNoteNumber();
	switch (status)
	{
		case EMidiStatusMsg::NoteOn:
		{
			event.mValue  = msg.GetVelocity();
			event.mAction = (event.mValue == 0) ? kNoteOffAction : kNoteOnAction;
			break;
		}
		case EMidiStatusMsg::NoteOff:
		{
			event.mAction = kNoteOffAction;
			event.mValue  = msg.GetVelocity();
			break;
		}
		case EMidiStatusMsg::PolyphonicAftertouch:
		{
			event.mAction = kPressureAction;
			event.mValue  = msg.GetPolyphonicAftertouch();
			break;
		}
		case EMidiStatusMsg::ChannelAftertouch:
		{
			event.mAction = kPressureAction;
			event.mValue  = msg.GetChannelAftertouch();
			break;
		}
		case EMidiStatusMsg::PitchBendChange:
		{
			event.mAction   = kPitchBendAction;
			float bendRange = mChannelStates[event.mAddress.mChannel].pitchBendRange;
			event.mValue    = msg.GetPitchWheel() * bendRange / 12.f;
			break;
		}
		case EMidiStatusMsg::ControlChange:
		{
			auto index              = msg.GetControlChangeIdx();
			event.mControllerNumber = +index;
			event.mValue            = msg.GetControlChange(index);
			switch (index)
			{
				// handle special controllers
				case EMidiControlChangeMsg::SoundControllerBrightness:
				{
					event.mAction = kTimbreAction;
					break;
				}
				case EMidiControlChangeMsg::AllNotesOff:
				{
					event.mAddress.mFlags = kVoicesAll;
					event.mAction         = kNoteOffAction;
					break;
				}
				// handle all other controllers
				default:
				{
					event.mAction = kControllerAction;
					break;
				}
			}
			break;
		}
		case EMidiStatusMsg::ProgramChange:
		{
			event.mAction           = kProgramChangeAction;
			event.mControllerNumber = msg.GetProgram();
			break;
		}
		default:
		{
			break;
		}
	}

	return event;
}

// Here we handle the MIDI messages used by MPE as listed in the MPE spec, page 7
VoiceInputEvent MidiSynth::MidiMessageToEventMPE(const IMidiMsg& msg)
{
	VoiceInputEvent event {};

	auto status             = msg.GetStatus();
	auto index              = msg.GetControlChangeIdx();
	event.mSampleOffset     = msg.mOffset;
	event.mAddress.mChannel = msg.GetChannel();
	event.mAddress.mKey     = msg.GetNoteNumber();
	event.mAddress.mZone    = MasterZoneFor(event.mAddress.mChannel);

	// handle pitch bend, channel pressure and CC#74 in the same way:
	// sum main and member channel values
	const bool isPitchBend       = (status == EMidiStatusMsg::PitchBendChange);
	const bool isChannelPressure = (status == EMidiStatusMsg::ChannelAftertouch);
	const bool isTimbre =
		(status == EMidiStatusMsg::ControlChange) && (index == EMidiControlChangeMsg::SoundControllerBrightness);
	if (isPitchBend || isChannelPressure || isTimbre)
	{
		float* pChannelDestValue {};
		float masterChannelStoredValue {};
		if (isPitchBend)
		{
			event.mAction   = kPitchBendAction;
			float bendRange = mChannelStates[event.mAddress.mChannel].pitchBendRange;
			event.mValue    = msg.GetPitchWheel() * bendRange / 12.f;

			pChannelDestValue        = &(mChannelStates[event.mAddress.mChannel].pitchBend);
			masterChannelStoredValue = mChannelStates[MasterChannelFor(event.mAddress.mChannel)].pitchBend;
		}
		else if (isChannelPressure)
		{
			event.mAction            = kPressureAction;
			event.mValue             = msg.GetChannelAftertouch();
			pChannelDestValue        = &(mChannelStates[event.mAddress.mChannel].pressure);
			masterChannelStoredValue = mChannelStates[MasterChannelFor(event.mAddress.mChannel)].pressure;
		}
		else if (isTimbre)
		{
			event.mAction            = kTimbreAction;
			event.mValue             = msg.GetControlChange(msg.GetControlChangeIdx());
			pChannelDestValue        = &(mChannelStates[event.mAddress.mChannel].timbre);
			masterChannelStoredValue = mChannelStates[MasterChannelFor(event.mAddress.mChannel)].timbre;
		}

		if (IsMasterChannel(event.mAddress.mChannel))
		{
			// store value in master channel
			*pChannelDestValue = event.mValue;

			// no action needed
			event.mAction = kNullAction;  // Swedish translation incoming... 3... 2... 1.... **** action...
		}
		else
		{
			// add stored master channel value to event value
			event.mValue += masterChannelStoredValue;

			// store sum in member channel
			*pChannelDestValue = event.mValue;
		}
		return event;
	}

	// pitch bend sensitivity (RPN 0) is handled in HandleRPN()

	// poly key pressure is ignored in MPE.

	switch (status)
	{
		// program change:
		// we are using MIDI mode 3. A program change sent to a master channel
		// affects all voices within the zone. Program changes sent to member channels are ignored.
		case EMidiStatusMsg::ProgramChange:
		{
			if (IsMasterChannel(event.mAddress.mChannel))
			{
				event.mAction           = kProgramChangeAction;
				event.mControllerNumber = msg.GetProgram();
			}
			else
				event.mAction = kNullAction;
			break;
		}
		case EMidiStatusMsg::NoteOn:
		{
			event.mValue  = msg.GetVelocity();
			event.mAction = (event.mValue == 0) ? kNoteOffAction : kNoteOnAction;
			break;
		}
		case EMidiStatusMsg::NoteOff:
		{
			event.mAction = kNoteOffAction;
			event.mValue  = msg.GetVelocity();
			break;
		}
		case EMidiStatusMsg::ControlChange:
		{
			index                   = msg.GetControlChangeIdx();
			event.mControllerNumber = +index;
			if (index == EMidiControlChangeMsg::AllNotesOff)
			{
				event.mAddress.mFlags = kVoicesAll;
				event.mAction         = kNoteOffAction;
			}
			else
			{
				// send all other controllers to matching channels using the generic control action
				// note: according to the MPE spec these messages should be sent to all channels in the zone,
				// but that is less useful IMO - to do so just add the line
				// event.mAddress.mChannel = kAllChannels;
				event.mAction = kControllerAction;
			}
			event.mValue = msg.GetControlChange(index);
			break;
		}
		default:
		{
			break;
		}
	}

	return event;
}

VoiceInputEvent MidiSynth::MidiMessageToEvent(const IMidiMsg& msg)
{
	return (mMPEMode ? MidiMessageToEventMPE(msg) : MidiMessageToEventBasic(msg));
}

// sets the number of channels in the lo or hi MPE zones.
void MidiSynth::SetMPEZones(const int channel, const int nChans)
{
	DEBUG_ASSERT(math::ClampEval(channel, 0, 15));
	DEBUG_ASSERT(math::ClampEval(nChans, 0, 15));

	// total channels = member channels + the master channel, or 0 if there is no Zone. totalChannels is never 1.
	const int totalChannels = nChans ? (nChans + 1) : 0;

	if (channel == 0)
	{
		mMPELowerZoneChannels = totalChannels;
		mMPEUpperZoneChannels = math::Clamp(mMPEUpperZoneChannels, 0, 16 - mMPELowerZoneChannels);
	}
	else if (channel == 15)
	{
		mMPEUpperZoneChannels = totalChannels;
		mMPELowerZoneChannels = math::Clamp(mMPELowerZoneChannels, 0, 16 - mMPEUpperZoneChannels);
	}

	// activate / deactivate MPE mode if needed
	bool anyMPEChannelsActive = (mMPELowerZoneChannels || mMPEUpperZoneChannels);
	if (anyMPEChannelsActive && (!mMPEMode))
		mMPEMode = true;
	else if ((!anyMPEChannelsActive) && (mMPEMode))
		mMPEMode = false;

	// reset pitch bend ranges as per MPE spec
	if (mMPEMode)
	{
		if (channel == 0)
		{
			SetChannelPitchBendRange(kMPELowerZoneMasterChannel, 2);
			SetChannelPitchBendRange(kMPELowerZoneMasterChannel + 1, 48);
		}
		else if (channel == 15)
		{
			SetChannelPitchBendRange(kMPEUpperZoneMasterChannel, 2);
			SetChannelPitchBendRange(kMPEUpperZoneMasterChannel - 1, 48);
		}
	}
	else
		SetPitchBendRange(mNonMPEPitchBendRange);

	//	std::cout << "MPE mode: " << (mMPEMode ? "ON" : "OFF") << "\n";
	//	std::cout << "MPE channels: \n    lo: " << mMPELowerZoneChannels << " hi " << mMPEUpperZoneChannels << "\n";
}

void MidiSynth::SetChannelPitchBendRange(int channelParam, uint8 rangeParam)
{
	int channelLo, channelHi;
	if (IsInLowerZone(channelParam))
	{
		channelLo = LowerZoneStart();
		channelHi = LowerZoneEnd();
	}
	else if (IsInUpperZone(channelParam))
	{
		channelLo = UpperZoneStart();
		channelHi = UpperZoneEnd();
	}
	else
	{
		channelLo = channelHi = math::Clamp(channelParam, 0, 15);
	}

	uint8 range = math::Clamp<uint8>(rangeParam, 0, 96);

	for (int i = channelLo; i <= channelHi; ++i)
	{
		mChannelStates[i].pitchBendRange = range;
	}
}

bool IsRPNMessage(IMidiMsg msg)
{
	if (msg.GetStatus() != EMidiStatusMsg::ControlChange)
		return false;

	switch (msg.GetControlChangeIdx())
	{
		case EMidiControlChangeMsg::DataEntry:
		case EMidiControlChangeMsg::LSBDataEntry:
		case EMidiControlChangeMsg::RegisteredParameterNumberLSB:
		case EMidiControlChangeMsg::RegisteredParameterNumberMSB:
			return true;
		default:
			return false;
	}
}

void MidiSynth::HandleRPN(IMidiMsg msg)
{
	int channel         = msg.GetChannel();
	ChannelState& state = mChannelStates[channel];

	uint8_t valueByte = msg.mData2;
	int param, value;
	switch (static_cast<EMidiControlChangeMsg>(msg.mData1))
	{
		case EMidiControlChangeMsg::RegisteredParameterNumberLSB:
			state.paramLSB = valueByte;
			state.valueMSB = state.valueLSB = 0xff;
			break;
		case EMidiControlChangeMsg::RegisteredParameterNumberMSB:
			state.paramMSB = valueByte;
			state.valueMSB = state.valueLSB = 0xff;
			break;
		case EMidiControlChangeMsg::LSBDataEntry:
			state.valueLSB = valueByte;
			break;
		case EMidiControlChangeMsg::DataEntry:
			// whenever the value MSB byte is received we constuct the value and take action on the RPN.
			// if only the MSB has been received, it is used as the entire value so the maximum possible value is 127.
			state.valueMSB = valueByte;
			param          = ((state.paramMSB & 0xFF) << 7) + (state.paramLSB & 0xFF);

			if (state.valueLSB != 0xff)
				value = ((state.valueMSB & 0xFF) << 7) + (state.valueLSB & 0xFF);
			else
				value = state.valueMSB & 0xFF;

			//			std::cout << "RPN received: channel " << channel << ", param " << param << ", value " << value
			//<<
			//"\n";
			switch (param)
			{
				case 0:  // RPN 0 : pitch bend range
					SetChannelPitchBendRange(channel, static_cast<uint8>(value));
					break;
				case 6:  // RPN 6 : MPE zone configuration. These messages may turn MPE mode on or off.
					if (IsMasterChannel(channel))
					{
						SetMPEZones(channel, value);
					}
					break;
				default:
					break;
			}
			break;

		default:
			break;
	}
}

bool MidiSynth::ProcessBlock(sample** inputs, sample** outputs, uint32 nInputs, uint32 nOutputs, uint32 nFrames)
{
	assert(NVoices());

	if (mVoicesAreActive | !mMidiQueue.Empty())
	{
		int blockSize        = mBlockSize;
		int samplesRemaining = nFrames;
		int startIndex       = 0;

		while (samplesRemaining > 0)
		{
			if (samplesRemaining < blockSize)
				blockSize = samplesRemaining;

			while (!mMidiQueue.Empty())
			{
				IMidiMsg msg = mMidiQueue.Peek();

				// we assume the messages are in chronological order. If we find one later than the current block we are
				// done.
				if (msg.mOffset > startIndex + blockSize)
					break;

				if (IsRPNMessage(msg))
				{
					HandleRPN(msg);
				}
				else
				{
					// send performance messages to the voice allocator
					// message offset is relative to the start of this processSamples() block
					msg.mOffset -= startIndex;
					mVoiceAllocator.AddEvent(MidiMessageToEvent(msg));
				}
				mMidiQueue.Remove();
			}

			mVoiceAllocator.ProcessEvents(blockSize, mSampleTime);
			mVoiceAllocator.ProcessVoices(inputs, outputs, nInputs, nOutputs, startIndex, blockSize);

			samplesRemaining -= blockSize;
			startIndex += blockSize;
			mSampleTime += blockSize;
		}

		bool voicesbusy = false;
		int activeCount = 0;

		for (int v = 0; v < NVoices(); v++)
		{
			bool busy = GetVoice(v)->GetBusy();
			voicesbusy |= busy;

			activeCount += (busy == true);
			if constexpr (Config::Debug::MidiSynthShowVoiceCount)
			{
				if (GetVoice(v)->GetBusy())
					printf("X");
				else
					DBGMSG("_");
			}
		}
		if constexpr (Config::Debug::MidiSynthShowVoiceCount)
			DBGMSG("\nNum Voices busy %i\n", activeCount);

		mVoicesAreActive = voicesbusy;

		mMidiQueue.Flush(nFrames);
	}
	else  // empty block
	{
		return true;
	}

	return false;  // made some noise
}

void MidiSynth::SetSampleRateAndBlockSize(double sampleRate, int blockSize)
{
	Reset();

	mSampleRate = sampleRate;
	mMidiQueue.Resize(blockSize);
	mVoiceAllocator.SetSampleRateAndBlockSize(sampleRate, blockSize);

	for (int v = 0; v < NVoices(); v++)
	{
		GetVoice(v)->SetSampleRateAndBlockSize(sampleRate, blockSize);
	}
}
