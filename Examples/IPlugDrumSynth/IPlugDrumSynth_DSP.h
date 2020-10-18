#pragma once

#include "Extras/Oscillator.h"
#include "Extras/ADSREnvelope.h"

using namespace iplug;

inline static constexpr int kNumDrums          = 4;
inline static constexpr tfloat kStartFreq      = 300.;  // Hz
inline static constexpr tfloat kFreqDiff       = 100.;  // Hz
inline static constexpr tfloat kPitchEnvRange  = 100.;  // Hz
inline static constexpr tfloat kAmpDecayTime   = 300;   // Ms
inline static constexpr tfloat kPitchDecayTime = 50.;   // Ms


class DrumSynthDSP
{
 public:
	struct DrumVoice
	{
		ADSREnvelope<> mPitchEnv {"pitch", nullptr, false};
		ADSREnvelope<> mAmpEnv {"amp", nullptr, false};
		FastSinOscillator<> mOsc;
		tfloat mBaseFreq;

		DrumVoice(tfloat baseFreq) : mBaseFreq(baseFreq)
		{
			mAmpEnv.SetStageTime(ADSREnvelope<>::EStage::kAttack, 0.);
			mAmpEnv.SetStageTime(ADSREnvelope<>::EStage::kDecay, kAmpDecayTime);

			mPitchEnv.SetStageTime(ADSREnvelope<>::EStage::kAttack, 0.);
			mPitchEnv.SetStageTime(ADSREnvelope<>::EStage::kDecay, kPitchDecayTime);
		}

		inline tfloat Process()
		{
			return mOsc.Process(mBaseFreq + mPitchEnv.Process()) * mAmpEnv.Process();
		}

		void Trigger(tfloat amp)
		{
			mOsc.Reset();
			mPitchEnv.Start(amp * kPitchEnvRange);
			mAmpEnv.Start(amp);
		}

		inline bool IsActive() const
		{
			return mAmpEnv.GetBusy();
		}
	};

	DrumSynthDSP()
	//  : mMidiQueue(IMidiMsg::QueueSize(DEFAULT_BLOCK_SIZE, DEFAULT_SAMPLE_RATE))
	{
		for (int d = 0; d < kNumDrums; d++)
		{
			mDrums.push_back(DrumVoice(kStartFreq + (d * kFreqDiff)));
		}
	}

	void Reset(tfloat sampleRate, int blockSize)
	{
		mMidiQueue.Resize(blockSize);
		//    mMidiQueue.Resize(IMidiMsg::QueueSize(blockSize, sampleRate));
	}

	void ProcessBlock(sample** outputs, int nFrames)
	{
		for (int s = 0; s < nFrames; s++)
		{
			while (!mMidiQueue.Empty())
			{
				IMidiMsg& msg = mMidiQueue.Peek();
				if (msg.mOffset > s)
					break;

				if (msg.GetStatus() == EMidiStatusMsg::NoteOn && msg.GetVelocity())
				{
					int pitchClass = msg.NoteNumber() % 12;

					if (pitchClass < kNumDrums)
						mDrums[pitchClass].Trigger(msg.GetVelocity());
				}

				mMidiQueue.Remove();
			}

			if (mMultiOut)
			{
				int channel = 0;
				for (int d = 0; d < kNumDrums; d++)
				{
					outputs[channel][s] = 0.;

					if (mDrums[d].IsActive())
						outputs[channel][s] = mDrums[d].Process();

					outputs[channel + 1][s] = outputs[channel][s];

					channel += 2;
				}
			}
			else
			{
				outputs[0][s] = 0.;

				for (int d = 0; d < kNumDrums; d++)
				{
					if (mDrums[d].IsActive())
						outputs[0][s] += mDrums[d].Process();
				}

				outputs[1][s] = outputs[0][s];
			}
		}
		mMidiQueue.Flush(nFrames);
	}

	void ProcessMidiMsg(const IMidiMsg& msg)
	{
		mMidiQueue.Add(msg);
	}

	void SetMultiOut(bool multiOut)
	{
		mMultiOut = multiOut;
	}

 private:
	bool mMultiOut = false;
	std::vector<DrumVoice> mDrums;
	IMidiQueue mMidiQueue;
};
