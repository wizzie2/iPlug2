#include "IPlugInstrument.h"
#include "IPlug_include_in_plug_src.h"
#include "Extras/LFO.h"

IPlugInstrument::IPlugInstrument(const InstanceInfo& info) : Plugin(info, Config(kNumParams, kNumPresets))
{
	GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
	GetParam(kParamNoteGlideTime)->InitMilliseconds("Note Glide Time", 0., 0.0, 30.);
	GetParam(kParamAttack)
		->InitDouble("Attack", 10., 1., 1000., 0.1, "ms", IParam::kFlagsNone, "ADSR", IParam::ShapePowCurve(3.));
	GetParam(kParamDecay)
		->InitDouble("Decay", 10., 1., 1000., 0.1, "ms", IParam::kFlagsNone, "ADSR", IParam::ShapePowCurve(3.));
	GetParam(kParamSustain)->InitDouble("Sustain", 50., 0., 100., 1, "%", IParam::kFlagsNone, "ADSR");
	GetParam(kParamRelease)->InitDouble("Release", 10., 2., 1000., 0.1, "ms", IParam::kFlagsNone, "ADSR");
	GetParam(kParamLFOShape)->InitEnum("LFO Shape", +LFO<>::EShape::kTriangle, LFO<>::LFO_SHAPE_VALIST);
	GetParam(kParamLFORateHz)->InitFrequency("LFO Rate", 1., 0.01, 40.);
	GetParam(kParamLFORateTempo)->InitEnum("LFO Rate", +LFO<>::ETempoDivison::k1, LFO<>::LFO_TEMPODIV_VALIST);
	GetParam(kParamLFORateMode)->InitBool("LFO Sync", true);
	GetParam(kParamLFODepth)->InitPercentage("LFO Depth");

#if IPLUG_EDITOR  // http://bit.ly/2S64BDd
	mMakeGraphicsFunc = [&]() {
		return MakeGraphics(*this);
	};

	mLayoutFunc = [&](IGraphics* pGraphics) {
		pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
		pGraphics->AttachPanelBackground(COLOR_GRAY);
		pGraphics->EnableMouseOver(true);
		pGraphics->EnableMultiTouch(true);

		if constexpr (EPlugApi::Native == EPlugApi::WEB)
			pGraphics->AttachPopupMenuControl();

		//    pGraphics->EnableLiveEdit(true);
		pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
		const IRECT b        = pGraphics->GetBounds().GetPadded(-20.f);
		const IRECT lfoPanel = b.GetFromLeft(300.f).GetFromTop(200.f);
		IRECT keyboardBounds = b.GetFromBottom(300);
		IRECT wheelsBounds   = keyboardBounds.ReduceFromLeft(100.f).GetPadded(-10.f);
		pGraphics->AttachControl(new IVKeyboardControl(keyboardBounds), kCtrlTagKeyboard);
		pGraphics->AttachControl(new IWheelControl(wheelsBounds.FracRectHorizontal(0.5)), kCtrlTagBender);
		pGraphics->AttachControl(
			new IWheelControl(wheelsBounds.FracRectHorizontal(0.5, true), EMidiControlChangeMsg::ModulationWheel));
		//    pGraphics->AttachControl(new IVMultiSliderControl<4>(b.GetGridCell(0, 2, 2).GetPadded(-30), "",
		//    DEFAULT_STYLE, kParamAttack, EDirection::Vertical, 0.f, 1.f));
		const IRECT controls = b.GetGridCell(1, 2, 2);
		pGraphics->AttachControl(
			new IVKnobControl(controls.GetGridCell(0, 2, 6).GetCentredInside(90), kParamGain, "Gain"));
		pGraphics->AttachControl(
			new IVKnobControl(controls.GetGridCell(1, 2, 6).GetCentredInside(90), kParamNoteGlideTime, "Glide"));
		const IRECT sliders =
			controls.GetGridCell(2, 2, 6).Union(controls.GetGridCell(3, 2, 6)).Union(controls.GetGridCell(4, 2, 6));
		pGraphics->AttachControl(
			new IVSliderControl(sliders.GetGridCell(0, 1, 4).GetMidHPadded(30.), kParamAttack, "Attack"));
		pGraphics->AttachControl(
			new IVSliderControl(sliders.GetGridCell(1, 1, 4).GetMidHPadded(30.), kParamDecay, "Decay"));
		pGraphics->AttachControl(
			new IVSliderControl(sliders.GetGridCell(2, 1, 4).GetMidHPadded(30.), kParamSustain, "Sustain"));
		pGraphics->AttachControl(
			new IVSliderControl(sliders.GetGridCell(3, 1, 4).GetMidHPadded(30.), kParamRelease, "Release"));
		pGraphics->AttachControl(new IVLEDMeterControl<2>(controls.GetFromRight(100).GetPadded(-30)), kCtrlTagMeter);

		pGraphics
			->AttachControl(
				new IVKnobControl(lfoPanel.GetGridCell(0, 0, 2, 3).GetCentredInside(60), kParamLFORateHz, "Rate"),
				kNoTag,
				"LFO")
			->Hide(true);
		pGraphics
			->AttachControl(
				new IVKnobControl(lfoPanel.GetGridCell(0, 0, 2, 3).GetCentredInside(60), kParamLFORateTempo, "Rate"),
				kNoTag,
				"LFO")
			->DisablePrompt(false);
		pGraphics->AttachControl(
			new IVKnobControl(lfoPanel.GetGridCell(0, 1, 2, 3).GetCentredInside(60), kParamLFODepth, "Depth"),
			kNoTag,
			"LFO");
		pGraphics
			->AttachControl(
				new IVKnobControl(lfoPanel.GetGridCell(0, 2, 2, 3).GetCentredInside(60), kParamLFOShape, "Shape"),
				kNoTag,
				"LFO")
			->DisablePrompt(false);
		pGraphics
			->AttachControl(
				new IVSlideSwitchControl(
					lfoPanel.GetGridCell(1, 0, 2, 3).GetFromTop(30).GetMidHPadded(20),
					kParamLFORateMode,
					"Sync",
					DEFAULT_STYLE.WithShowValue(false).WithShowLabel(false).WithWidgetFrac(0.5f).WithDrawShadows(false),
					false),
				kNoTag,
				"LFO")
			->SetAnimationEndActionFunction([pGraphics](IControl* pControl) {
				bool sync = pControl->GetValue() > 0.5;
				pGraphics->HideControl(kParamLFORateHz, sync);
				pGraphics->HideControl(kParamLFORateTempo, !sync);
			});
		pGraphics->AttachControl(
			new IVDisplayControl(lfoPanel.GetGridCell(1, 1, 2, 3).Union(lfoPanel.GetGridCell(1, 2, 2, 3)),
								 "",
								 DEFAULT_STYLE,
								 EDirection::Horizontal,
								 0.f,
								 1.f,
								 0.f,
								 1024),
			kCtrlTagLFOVis,
			"LFO");

		pGraphics->AttachControl(new IVGroupControl("LFO", "LFO", 10.f, 20.f, 10.f, 10.f));

		pGraphics
			->AttachControl(
				new IVButtonControl(keyboardBounds.GetFromTRHC(200, 30).GetTranslated(0, -30),
									SplashClickActionFunc,
									"Show/Hide Keyboard",
									DEFAULT_STYLE.WithColor(kFG, COLOR_WHITE).WithLabelText({15.f, EVAlign::Middle})))
			->SetAnimationEndActionFunction([pGraphics](IControl* pCaller) {
				static bool hide = false;
				pGraphics->GetControlWithTag(kCtrlTagKeyboard)->Hide(hide = !hide);
				pGraphics->Resize(
					Config::plugWidth, hide ? Config::plugHeight / 2 : Config::plugHeight, pGraphics->GetDrawScale());
			});


	#if PLATFORM_IOS
		if (!IsAuv3AppExtension())
		{
			pGraphics->AttachControl(new IVButtonControl(
				b.GetFromTRHC(100, 100),
				[pGraphics](IControl* pCaller) {
					dynamic_cast<IGraphicsIOS*>(pGraphics)->LaunchBluetoothMidiDialog(pCaller->GetRECT().L,
																					  pCaller->GetRECT().MH());
					SplashClickActionFunc(pCaller);
				},
				"BTMIDI"));
		}
	#endif

		pGraphics->SetQwertyMidiKeyHandlerFunc([pGraphics](const IMidiMsg& msg) {
			dynamic_cast<IVKeyboardControl*>(pGraphics->GetControlWithTag(kCtrlTagKeyboard))
				->SetNoteFromMidi(msg.GetNoteNumber(), msg.GetStatus() == EMidiStatusMsg::NoteOn);
		});
	};
#endif
}

#if IPLUG_EDITOR
bool IPlugInstrument::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
	if (ctrlTag == kCtrlTagBender && msgTag == IWheelControl::kMessageTagSetPitchBendRange)
	{
		const int bendRange = *static_cast<const int*>(pData);
		mDSP.mSynth.SetPitchBendRange(bendRange);
	}

	return false;
}
#endif


#if IPLUG_DSP
void IPlugInstrument::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
	mDSP.ProcessBlock(
		nullptr, outputs, 2, nFrames, static_cast<sample>(mTimeInfo.mPPQPos), mTimeInfo.mTransportIsRunning);
	mMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagMeter);
	mLFOVisSender.PushData({kCtrlTagLFOVis, {float(mDSP.mLFO.GetLastOutput())}});
}

void IPlugInstrument::OnIdle()
{
	mMeterSender.TransmitData(*this);
	mLFOVisSender.TransmitData(*this);
}

void IPlugInstrument::OnReset()
{
	mDSP.Reset(GetSampleRate(), GetBlockSize());
}

void IPlugInstrument::ProcessMidiMsg(const IMidiMsg& msg)
{
	EMidiStatusMsg status = msg.GetStatus();
	switch (status)
	{
		case EMidiStatusMsg::NoteOn:
		case EMidiStatusMsg::NoteOff:
		case EMidiStatusMsg::PolyphonicAftertouch:
		case EMidiStatusMsg::ControlChange:
		case EMidiStatusMsg::ProgramChange:
		case EMidiStatusMsg::ChannelAftertouch:
		case EMidiStatusMsg::PitchBendChange:
			mDSP.ProcessMidiMsg(msg);
			SendMidiMsg(msg);
			break;
	}
}

void IPlugInstrument::OnParamChange(int paramIdx)
{
	mDSP.SetParam(paramIdx, static_cast<sample>(GetParam(paramIdx)->Value()));
}
#endif
