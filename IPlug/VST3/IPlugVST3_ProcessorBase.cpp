/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
 */

#include "IPlugVST3_ProcessorBase.h"


using namespace iplug;

#ifndef CUSTOM_BUSTYPE_FUNC
uint64 iplug::GetAPIBusTypeForChannelIOConfig(
	int configIdx, ERoute dir, int busIdx, const IOConfig* pConfig, WDL_TypedBuf<uint64_t>* APIBusTypes)
{
	assert(pConfig != nullptr);
	assert(busIdx >= 0 && busIdx < pConfig->NBuses(dir));

	int numChans = pConfig->GetBusInfo(dir, busIdx)->NChans();

	switch (numChans)
	{
		case 0:
			return Steinberg::Vst::SpeakerArr::kEmpty;
		case 1:
			return Steinberg::Vst::SpeakerArr::kMono;
		case 2:
			return Steinberg::Vst::SpeakerArr::kStereo;
		case 3:
			return Steinberg::Vst::SpeakerArr::k30Cine;  // CHECK - not the same as protools
		case 4:
			return Steinberg::Vst::SpeakerArr::kAmbi1stOrderACN;
		case 5:
			return Steinberg::Vst::SpeakerArr::k50;
		case 6:
			return Steinberg::Vst::SpeakerArr::k51;
		case 7:
			return Steinberg::Vst::SpeakerArr::k70Cine;
		case 8:
			return Steinberg::Vst::SpeakerArr::k71CineSideFill;  // CHECK - not the same as protools
		case 9:
			return Steinberg::Vst::SpeakerArr::kAmbi2cdOrderACN;
		case 10:
			return Steinberg::Vst::SpeakerArr::k71_2;  // aka k91Atmos
		case 16:
			return Steinberg::Vst::SpeakerArr::kAmbi3rdOrderACN;
		default:
			DBGMSG("do not yet know what to do here\n");
			assert(0);
			return Steinberg::Vst::SpeakerArr::kEmpty;
	}
}
#endif

IPlugVST3ProcessorBase::IPlugVST3ProcessorBase(Config c, IPlugAPIBase& plug)
	: IPlugProcessor(c, EPlugApi::VST3)
	, mPlug(plug)
{
	SetChannelConnections(ERoute::kInput, 0, MaxNChannels(ERoute::kInput), true);
	SetChannelConnections(ERoute::kOutput, 0, MaxNChannels(ERoute::kOutput), true);

	mMaxNChansForMainInputBus = MaxNChannelsForBus(ERoute::kInput, 0);

	if (MaxNChannels(ERoute::kInput))
	{
		mLatencyDelay = std::unique_ptr<NChanDelayLine<PLUG_SAMPLE_DST>>(
			new NChanDelayLine<PLUG_SAMPLE_DST>(MaxNChannels(ERoute::kInput), MaxNChannels(ERoute::kOutput)));
		mLatencyDelay->SetDelayTime(GetLatency());
	}

	// Make sure the process context is predictably initialised in case it is used before process is called
	memset(&mProcessContext, 0, sizeof(Steinberg::Vst::ProcessContext));
}

void IPlugVST3ProcessorBase::ProcessMidiIn(Steinberg::Vst::IEventList* pEventList,
										   IPlugQueue<IMidiMsg>& editorQueue,
										   IPlugQueue<IMidiMsg>& processorQueue)
{
	IMidiMsg msg;

	if (pEventList)
	{
		int32 numEvent = pEventList->getEventCount();
		for (int32 i = 0; i < numEvent; i++)
		{
			Steinberg::Vst::Event event;
			if (pEventList->getEvent(i, event) == Steinberg::kResultOk)
			{
				switch (event.type)
				{
					case Steinberg::Vst::Event::kNoteOnEvent:
					{
						msg.SetNoteOn(
							event.noteOn.pitch, event.noteOn.velocity, event.sampleOffset, event.noteOn.channel);
						ProcessMidiMsg(msg);
						processorQueue.Push(msg);
						break;
					}

					case Steinberg::Vst::Event::kNoteOffEvent:
					{
						msg.SetNoteOff(event.noteOff.pitch, event.sampleOffset, event.noteOff.channel);
						ProcessMidiMsg(msg);
						processorQueue.Push(msg);
						break;
					}
					case Steinberg::Vst::Event::kPolyPressureEvent:
					{
						msg.SetPolyphonicAftertouch(event.polyPressure.pitch,
										  event.polyPressure.pressure * 127.,
										  event.sampleOffset,
										  event.polyPressure.channel);
						ProcessMidiMsg(msg);
						processorQueue.Push(msg);
						break;
					}
					case Steinberg::Vst::Event::kDataEvent:
					{
						ISysEx syx = ISysEx(event.sampleOffset, event.data.bytes, event.data.size);
						ProcessSysEx(syx);
						break;
					}
				}
			}
		}
	}

	while (editorQueue.Pop(msg))
	{
		ProcessMidiMsg(msg);
	}
}

void IPlugVST3ProcessorBase::ProcessMidiOut(IPlugQueue<SysExData>& sysExQueue,
											SysExData& sysExBuf,
											Steinberg::Vst::IEventList* pOutputEvents,
											Steinberg::int32 numSamples)
{
	if (!mMidiOutputQueue.Empty() && pOutputEvents)
	{
		Steinberg::Vst::Event toAdd = {0};
		IMidiMsg msg;

		while (!mMidiOutputQueue.Empty())
		{
			IMidiMsg& msg = mMidiOutputQueue.Peek();

			if (msg.GetStatus() == EMidiStatusMsg::NoteOn)
			{
				Steinberg::Vst::Helpers::init(toAdd, Steinberg::Vst::Event::kNoteOnEvent, 0 /*bus id*/, msg.mOffset);

				toAdd.noteOn.channel  = msg.GetChannel();
				toAdd.noteOn.pitch    = msg.GetNoteNumber();
				toAdd.noteOn.tuning   = 0.;
				toAdd.noteOn.velocity = (float) msg.GetVelocity() * (1.f / 127.f);
				pOutputEvents->addEvent(toAdd);
			}
			else if (msg.GetStatus() == EMidiStatusMsg::NoteOff)
			{
				Steinberg::Vst::Helpers::init(toAdd, Steinberg::Vst::Event::kNoteOffEvent, 0 /*bus id*/, msg.mOffset);

				toAdd.noteOff.channel  = msg.GetChannel();
				toAdd.noteOff.pitch    = msg.GetNoteNumber();
				toAdd.noteOff.velocity = (float) msg.GetVelocity() * (1.f / 127.f);
				pOutputEvents->addEvent(toAdd);
			}
			else if (msg.GetStatus() == EMidiStatusMsg::PolyphonicAftertouch)
			{
				Steinberg::Vst::Helpers::initLegacyMIDICCOutEvent(
					toAdd, Steinberg::Vst::ControllerNumbers::kCtrlPolyPressure, msg.GetChannel(), msg.mData1, msg.mData2);
				toAdd.sampleOffset = msg.mOffset;
				pOutputEvents->addEvent(toAdd);
			}
			else if (msg.GetStatus() == EMidiStatusMsg::ChannelAftertouch)
			{
				Steinberg::Vst::Helpers::initLegacyMIDICCOutEvent(
					toAdd, Steinberg::Vst::ControllerNumbers::kAfterTouch, msg.GetChannel(), msg.mData1, msg.mData2);
				toAdd.sampleOffset = msg.mOffset;
				pOutputEvents->addEvent(toAdd);
			}
			else if (msg.GetStatus() == EMidiStatusMsg::ProgramChange)
			{
				Steinberg::Vst::Helpers::initLegacyMIDICCOutEvent(
					toAdd, Steinberg::Vst::ControllerNumbers::kCtrlProgramChange, msg.GetChannel(), msg.GetProgram(), 0);
				toAdd.sampleOffset = msg.mOffset;
				pOutputEvents->addEvent(toAdd);
			}
			else if (msg.GetStatus() == EMidiStatusMsg::ControlChange)
			{
				Steinberg::Vst::Helpers::initLegacyMIDICCOutEvent(
					toAdd, msg.mData1, msg.GetChannel(), msg.mData2, 0 /* value2?*/);
				toAdd.sampleOffset = msg.mOffset;
				pOutputEvents->addEvent(toAdd);
			}
			else if (msg.GetStatus() == EMidiStatusMsg::PitchBendChange)
			{
				toAdd.type                    = Steinberg::Vst::Event::kLegacyMIDICCOutEvent;
				toAdd.midiCCOut.channel       = msg.GetChannel();
				toAdd.sampleOffset            = msg.mOffset;
				toAdd.midiCCOut.controlNumber = Steinberg::Vst::ControllerNumbers::kPitchBend;

				 //int16 tmp             = static_cast<int16>(msg.GetPitchWheel() * 0x3FFF);
				 //toAdd.midiCCOut.value  = tmp & 0x7F;
				 //toAdd.midiCCOut.value2 = (tmp >> 7) & 0x7F;
				toAdd.midiCCOut.value  = msg.mData1;
				toAdd.midiCCOut.value2 = msg.mData2;

				pOutputEvents->addEvent(toAdd);
			}

			mMidiOutputQueue.Remove();
		}
	}

	mMidiOutputQueue.Flush(numSamples);

	// Output SYSEX from the editor, which has bypassed the processors' ProcessSysEx()
	if (sysExQueue.ElementsAvailable())
	{
		Steinberg::Vst::Event toAdd = {0};

		while (sysExQueue.Pop(sysExBuf))
		{
			toAdd.type         = Steinberg::Vst::Event::kDataEvent;
			toAdd.sampleOffset = sysExBuf.mOffset;
			toAdd.data.type    = Steinberg::Vst::DataEvent::kMidiSysEx;
			toAdd.data.size    = sysExBuf.mSize;

			// TODO!  this is a problem if more than one message in this block!
			toAdd.data.bytes = (uint8*) sysExBuf.mData;

			pOutputEvents->addEvent(toAdd);
		}
	}
}

void IPlugVST3ProcessorBase::AttachBuffers(
	ERoute direction, int idx, int n, Steinberg::Vst::AudioBusBuffers& pBus, int nFrames, Steinberg::int32 sampleSize)
{
	if (sampleSize == Steinberg::Vst::kSample32)
		IPlugProcessor::AttachBuffers(direction, idx, n, pBus.channelBuffers32, nFrames);
	else if (sampleSize == Steinberg::Vst::kSample64)
		IPlugProcessor::AttachBuffers(direction, idx, n, pBus.channelBuffers64, nFrames);
}

bool IPlugVST3ProcessorBase::SetupProcessing(const Steinberg::Vst::ProcessSetup& setup,
											 Steinberg::Vst::ProcessSetup& storedSetup)
{
	if ((setup.symbolicSampleSize != Steinberg::Vst::kSample32) &&
		(setup.symbolicSampleSize != Steinberg::Vst::kSample64))
		return false;

	storedSetup = setup;

	SetSampleRate(setup.sampleRate);

	// TODO: should IPlugVST3Processor call SetBlockSize in construct unlike other APIs?
	IPlugProcessor::SetBlockSize(setup.maxSamplesPerBlock);
	mMidiOutputQueue.Resize(setup.maxSamplesPerBlock);
	OnReset();

	return true;
}

bool IPlugVST3ProcessorBase::SetProcessing(bool state)
{
	if (!state)
		OnReset();

	return true;
}

bool IPlugVST3ProcessorBase::CanProcessSampleSize(Steinberg::int32 symbolicSampleSize)
{
	switch (symbolicSampleSize)
	{
		case Steinberg::Vst::kSample32:  // fall through
		case Steinberg::Vst::kSample64:
			return true;
		default:
			return false;
	}
}

void IPlugVST3ProcessorBase::PrepareProcessContext(Steinberg::Vst::ProcessData& data,
												   Steinberg::Vst::ProcessSetup& setup)
{
	ITimeInfo timeInfo;

	if (data.processContext)
		memcpy(&mProcessContext, data.processContext, sizeof(Steinberg::Vst::ProcessContext));

	if (mProcessContext.state & Steinberg::Vst::ProcessContext::kProjectTimeMusicValid)
		timeInfo.mSamplePos = (double) mProcessContext.projectTimeSamples;
	timeInfo.mPPQPos               = mProcessContext.projectTimeMusic;
	timeInfo.mTempo                = mProcessContext.tempo;
	timeInfo.mLastBar              = mProcessContext.barPositionMusic;
	timeInfo.mCycleStart           = mProcessContext.cycleStartMusic;
	timeInfo.mCycleEnd             = mProcessContext.cycleEndMusic;
	timeInfo.mNumerator            = mProcessContext.timeSigNumerator;
	timeInfo.mDenominator          = mProcessContext.timeSigDenominator;
	timeInfo.mTransportIsRunning   = mProcessContext.state & Steinberg::Vst::ProcessContext::kPlaying;
	timeInfo.mTransportLoopEnabled = mProcessContext.state & Steinberg::Vst::ProcessContext::kCycleActive;
	const bool offline             = setup.processMode == Steinberg::Vst::kOffline;
	SetTimeInfo(timeInfo);
	SetRenderingOffline(offline);
}

void IPlugVST3ProcessorBase::ProcessParameterChanges(Steinberg::Vst::ProcessData& data,
													 IPlugQueue<IMidiMsg>& fromProcessor)
{
	Steinberg::Vst::IParameterChanges* paramChanges = data.inputParameterChanges;

	if (!paramChanges)
		return;

	int32 numParamsChanged = paramChanges->getParameterCount();

	for (int32 i = 0; i < numParamsChanged; i++)
	{
		Steinberg::Vst::IParamValueQueue* paramQueue = paramChanges->getParameterData(i);
		if (!paramQueue)
			continue;

		int32 numPoints = paramQueue->getPointCount();
		Steinberg::int32 offsetSamples;
		double value;

		if (paramQueue->getPoint(numPoints - 1, offsetSamples, value) == Steinberg::kResultTrue)
		{
			uint32 idx = paramQueue->getParameterId();

			switch (idx)
			{
				case +EVST3ParamIDs::kBypassParam:
				{
					const bool bypassed = (value > 0.5);

					if (bypassed != GetBypassed())
						SetBypassed(bypassed);

					break;
				}
				default:
				{
					if (idx >= 0 && idx < static_cast<uint32>(mPlug.NParams()))
					{
#ifdef PARAMS_MUTEX
						mPlug.mParams_mutex.Enter();
#endif
						mPlug.GetParam(idx)->SetNormalized(value);

						// In VST3 non distributed the same parameter value is also set via
						// IPlugVST3Controller::setParamNormalized(ParamID tag, ParamValue value)
						mPlug.OnParamChange(idx, EParamSource::kHost, offsetSamples);
#ifdef PARAMS_MUTEX
						mPlug.mParams_mutex.Leave();
#endif
					}
					else if (idx >= +EVST3ParamIDs::kMIDICCParamStartIdx)
					{
						int index   = idx - +EVST3ParamIDs::kMIDICCParamStartIdx;
						uint8 channel = static_cast<uint8>(index / Steinberg::Vst::kCountCtrlNumber);

						EMidiControlChangeMsg ctrlr =
							static_cast<EMidiControlChangeMsg>(index % Steinberg::Vst::kCountCtrlNumber);

						IMidiMsg msg;

						if (+ctrlr == Steinberg::Vst::ControllerNumbers::kAfterTouch)
							msg.SetChannelAftertouch(static_cast<float>(value), offsetSamples, channel);
						else if (+ctrlr == Steinberg::Vst::ControllerNumbers::kPitchBend)
							msg.SetPitchWheel((value * 2) - 1, channel, offsetSamples);
						else
							msg.SetControlChange(ctrlr, value, channel, offsetSamples);

						fromProcessor.Push(msg);
						ProcessMidiMsg(msg);
					}
				}
				break;
			}
		}
	}
}

void IPlugVST3ProcessorBase::ProcessAudio(Steinberg::Vst::ProcessData& data,
										  Steinberg::Vst::ProcessSetup& setup,
										  const Steinberg::Vst::BusList& ins,
										  const Steinberg::Vst::BusList& outs)
{
	int32 sampleSize = setup.symbolicSampleSize;

	if (sampleSize == Steinberg::Vst::kSample32 || sampleSize == Steinberg::Vst::kSample64)
	{
		if (data.numInputs)
		{
			SetChannelConnections(ERoute::kInput, 0, MaxNChannels(ERoute::kInput), false);

			if (ins.size() > 1)
			{
				if (ins[1].get()->isActive())  // Sidechain is active
				{
					mSidechainActive = true;
					SetChannelConnections(ERoute::kInput, 0, data.inputs[0].numChannels, true);
					SetChannelConnections(ERoute::kInput, mMaxNChansForMainInputBus, data.inputs[1].numChannels, true);
				}
				else
				{
					if (mSidechainActive)
					{
						ZeroScratchBuffers();
						mSidechainActive = false;
					}

					SetChannelConnections(ERoute::kInput, 0, data.inputs[0].numChannels, true);
				}

				AttachBuffers(
					ERoute::kInput, 0, data.inputs[0].numChannels, data.inputs[0], data.numSamples, sampleSize);

				if (mSidechainActive)
					AttachBuffers(ERoute::kInput,
								  mMaxNChansForMainInputBus,
								  data.inputs[1].numChannels,
								  data.inputs[1],
								  data.numSamples,
								  sampleSize);
			}
			else
			{
				SetChannelConnections(ERoute::kInput, 0, MaxNChannels(ERoute::kInput), false);
				SetChannelConnections(ERoute::kInput, 0, data.inputs[0].numChannels, true);
				AttachBuffers(
					ERoute::kInput, 0, data.inputs[0].numChannels, data.inputs[0], data.numSamples, sampleSize);
			}
		}

		for (int outBus = 0, chanOffset = 0; outBus < data.numOutputs; outBus++)
		{
			int busChannels = data.outputs[outBus].numChannels;
			SetChannelConnections(
				ERoute::kOutput, chanOffset, busChannels, static_cast<bool>(outs[outBus].get()->isActive()));
			SetChannelConnections(ERoute::kOutput,
								  chanOffset + busChannels,
								  MaxNChannels(ERoute::kOutput) - (chanOffset + busChannels),
								  false);
			AttachBuffers(ERoute::kOutput, chanOffset, busChannels, data.outputs[outBus], data.numSamples, sampleSize);
			chanOffset += busChannels;
		}

		if (GetBypassed())
		{
			if (sampleSize == Steinberg::Vst::kSample32)
				PassThroughBuffers(0.f, data.numSamples);  // single precision
			else
				PassThroughBuffers(0.0, data.numSamples);  // double precision
		}
		else
		{
#ifdef PARAMS_MUTEX
			mPlug.mParams_mutex.Enter();
#endif
			if (sampleSize == Steinberg::Vst::kSample32)
				ProcessBuffers(0.f, data.numSamples);  // single precision
			else
				ProcessBuffers(0.0, data.numSamples);  // double precision
#ifdef PARAMS_MUTEX
			mPlug.mParams_mutex.Leave();
#endif
		}
	}
}

void IPlugVST3ProcessorBase::Process(Steinberg::Vst::ProcessData& data,
									 Steinberg::Vst::ProcessSetup& setup,
									 const Steinberg::Vst::BusList& ins,
									 const Steinberg::Vst::BusList& outs,
									 IPlugQueue<IMidiMsg>& fromEditor,
									 IPlugQueue<IMidiMsg>& fromProcessor,
									 IPlugQueue<SysExData>& sysExFromEditor,
									 SysExData& sysExBuf)
{
	PrepareProcessContext(data, setup);
	ProcessParameterChanges(data, fromProcessor);

	if (DoesMIDIIn())
	{
		ProcessMidiIn(data.inputEvents, fromEditor, fromProcessor);
	}

	ProcessAudio(data, setup, ins, outs);

	if (DoesMIDIOut())
	{
		ProcessMidiOut(sysExFromEditor, sysExBuf, data.outputEvents, data.numSamples);
	}
}

bool IPlugVST3ProcessorBase::SendMidiMsg(const IMidiMsg& msg)
{
	mMidiOutputQueue.Add(msg);
	return true;
}
