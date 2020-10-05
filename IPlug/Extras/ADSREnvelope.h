/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
 */

#pragma once

BEGIN_IPLUG_NAMESPACE

#ifdef DEBUG_ADSREnvelope
	#undef DEBUG_ADSREnvelope
	#define DEBUG_ADSREnvelope true
#else
#define DEBUG_ADSREnvelope false
#endif

template <class TFLOAT = tfloat, bool DEBUG_MODE = DEBUG_ADSREnvelope>
class ADSREnvelope
{
 public:
	enum class EStage
	{
		kReleasedToEndEarly  = -3,
		kReleasedToRetrigger = -2,
		kIdle                = -1,
		kAttack,
		kDecay,
		kSustain,
		kRelease
	};

	static constexpr TFLOAT EARLY_RELEASE_TIME     = 20.;                // ms
	static constexpr TFLOAT RETRIGGER_RELEASE_TIME = 3.;                 // ms
	static constexpr TFLOAT MIN_ENV_TIME_MS        = 0.022675736961451;  // 1 sample @44100
	static constexpr TFLOAT MAX_ENV_TIME_MS        = 60000.;
	static constexpr TFLOAT ENV_VALUE_LOW          = 0.000001;  // -120dB
	static constexpr TFLOAT ENV_VALUE_HIGH         = 0.999;

 private:
	const char* mName;
	EStage mStage                = EStage::kIdle;  // the current stage
	TFLOAT mEarlyReleaseIncr     = 0;
	TFLOAT mRetriggerReleaseIncr = 0;
	TFLOAT mAttackIncr           = 0;
	TFLOAT mDecayIncr            = 0;
	TFLOAT mReleaseIncr          = 0;
	TFLOAT mEnvValue             = 0;  // current normalized value of the envelope
	TFLOAT mLevel                = 0;  // envelope depth from velocity
	TFLOAT mReleaseLevel         = 0;  // the level when the env is released
	TFLOAT mNewStartLevel        = 0;  // envelope depth from velocity when retriggering
	TFLOAT mPrevResult           = 0;  // last value BEFORE velocity scaling
	TFLOAT mPrevOutput           = 0;  // last value AFTER velocity scaling
	TFLOAT mScalar               = 1;  // for key-follow scaling
	TFLOAT mSampleRate;
	bool mReleased       = true;
	bool mSustainEnabled = true;  // when false env is AD only

	std::function<void()> mResetFunc      = nullptr;  // reset func
	std::function<void()> mEndReleaseFunc = nullptr;  // end release func

 public:
	/** Constructs an ADSREnvelope object
	 * @param name string to identify the envelope in debug mode, when DEBUG_ENV=1 is set as a global preprocessor macro
	 * @param resetFunc A function to call when the envelope gets retriggered, called when the fade out ramp is at zero,
	 * useful for example to reset an oscillator's phase
	 * @param sustainEnabled if true the envelope is an ADSR envelope. If false, it's is an AD envelope (suitable for
	 * drums). */
	ADSREnvelope(const char* name = "", std::function<void()> resetFunc = nullptr, bool sustainEnabled = true)
		: mName(name)
		, mResetFunc(resetFunc)
		, mSustainEnabled(sustainEnabled)
	{
		SetSampleRate(44100);
	}

	/** Sets the time for a particular envelope stage
	 * @param stage The stage to set the time for /see EStage
	 * @param timeMS The time in milliseconds for that stage */
	constexpr void SetStageTime(EStage stage, TFLOAT timeMS)
	{
		switch (stage)
		{
			case EStage::kAttack:
				mAttackIncr =
					CalcIncrFromTimeLinear(math::Clamp(timeMS, MIN_ENV_TIME_MS, MAX_ENV_TIME_MS), mSampleRate);
				break;
			case EStage::kDecay:
				mDecayIncr = CalcIncrFromTimeExp(math::Clamp(timeMS, MIN_ENV_TIME_MS, MAX_ENV_TIME_MS), mSampleRate);
				break;
			case EStage::kRelease:
				mReleaseIncr = CalcIncrFromTimeExp(math::Clamp(timeMS, MIN_ENV_TIME_MS, MAX_ENV_TIME_MS), mSampleRate);
				break;
			default:
				// error
				break;
		}
	}

	/** @return /c true if the envelope is not idle */
	constexpr bool GetBusy() const
	{
		return mStage != EStage::kIdle;
	}

	/** @return the previously output value */
	constexpr TFLOAT GetPrevOutput() const
	{
		return mPrevOutput;
	}

	/** Trigger/Start the envelope
	 * @param level The overall depth of the envelope (usually linked to MIDI velocity)
	 * @param timeScalar Factor to scale the envelope's rates. Use this, for example to adjust the envelope stage rates
	 * based on the key pressed */
	constexpr void Start(TFLOAT level, TFLOAT timeScalar = 1.)
	{
		mStage    = EStage::kAttack;
		mEnvValue = 0.;
		mLevel    = level;
		mScalar   = 1 / timeScalar;
		mReleased = false;
	}

	/** Release the envelope */
	constexpr void Release()
	{
		mStage        = EStage::kRelease;
		mReleaseLevel = mPrevResult;
		mEnvValue     = 1.;
		mReleased     = true;
	}

	/** Retrigger the envelope. This method will cause the envelope to move to a "releasedToRetrigger" stage, which is a
	 * fast ramp to zero in RETRIGGER_RELEASE_TIME, used when voices are stolen to avoid clicks.
	 * @param newStartLevel the overall level when the envelope restarts (usually linked to MIDI velocity)
	 * @param timeScalar Factor to scale the envelope's rates. Use this, for example to adjust the envelope stage rates
	 * based on the key pressed */
	constexpr void Retrigger(TFLOAT newStartLevel, TFLOAT timeScalar = 1.)
	{
		mEnvValue      = 1.;
		mNewStartLevel = newStartLevel;
		mScalar        = 1. / timeScalar;
		mReleaseLevel  = mPrevResult;
		mStage         = EStage::kReleasedToRetrigger;
		mReleased      = false;

		if constexpr (DEBUG_MODE)
			DBGMSG("retrigger\n");
	}

	/** Kill the envelope
	 * @param hard If true, the envelope will get reset automatically, probably causing an audible glitch. If false,
	 * it's a "soft kill", which will fade out in EARLY_RELEASE_TIME */
	constexpr void Kill(bool hard)
	{
		if (hard)
		{
			if (mStage != EStage::kIdle)
			{
				mReleaseLevel = 0.;
				mStage        = EStage::kIdle;
				mEnvValue     = 0.;
			}

			if constexpr (DEBUG_MODE)
				DBGMSG("hard kill\n");
		}
		else
		{
			if (mStage != EStage::kIdle)
			{
				mReleaseLevel = mPrevResult;
				mStage        = EStage::kReleasedToEndEarly;
				mEnvValue     = 1.;
			}

			if constexpr (DEBUG_MODE)
				DBGMSG("soft kill\n");
		}
	}

	/** Set the sample rate for processing, with updates the early release time and retrigger release time coefficents.
	 * NOTE: you also need to think about updating the Attack, Decay and Release times when the sample rate changes
	 * @param sr SampleRate in samples per second */
	constexpr void SetSampleRate(TFLOAT sr)
	{
		mSampleRate           = sr;
		mEarlyReleaseIncr     = CalcIncrFromTimeLinear(EARLY_RELEASE_TIME, sr);
		mRetriggerReleaseIncr = CalcIncrFromTimeLinear(RETRIGGER_RELEASE_TIME, sr);
	}

	/** Sets a function to call when the envelope gets retriggered, called when the fade out ramp is at zero, useful for
	 * example to reset an oscillator's phase WARNING: don't call this on the audio thread, std::function can malloc
	 * @param func the reset function, or nullptr for none */
	constexpr void SetResetFunc(std::function<void()> func)
	{
		mResetFunc = func;
	}

	/** Sets a function to call when the envelope gets released, called when the ramp is at zero
	 * WARNING: don't call this on the audio thread, std::function can malloc
	 * @param func the release function, or nullptr for none */
	constexpr void SetEndReleaseFunc(std::function<void()> func)
	{
		mEndReleaseFunc = func;
	}

	/** Process the envelope, returning the value according to the current envelope stage
	 * @param sustainLevel Since the sustain level could be changed during processing, it is supplied as an argument, so
	 * that it can be smoothed extenally if nessecary, to avoid discontinuities */
	constexpr TFLOAT Process(TFLOAT sustainLevel = 0.)
	{
		TFLOAT result = 0.;

		switch (mStage)
		{
			case EStage::kIdle:
				result = mEnvValue;
				break;
			case EStage::kAttack:
				mEnvValue += (mAttackIncr * mScalar);
				if (mEnvValue > ENV_VALUE_HIGH || mAttackIncr == 0.)
				{
					mStage    = EStage::kDecay;
					mEnvValue = 1.;
				}
				result = mEnvValue;
				break;
			case EStage::kDecay:
				mEnvValue -= ((mDecayIncr * mEnvValue) * mScalar);
				result = (mEnvValue * (1 - sustainLevel)) + sustainLevel;
				if (mEnvValue < ENV_VALUE_LOW)
				{
					if (mSustainEnabled)
					{
						mStage    = EStage::kSustain;
						mEnvValue = 1.;
						result    = sustainLevel;
					}
					else
						Release();
				}
				break;
			case EStage::kSustain:
				result = sustainLevel;
				break;
			case EStage::kRelease:
				mEnvValue -= ((mReleaseIncr * mEnvValue) * mScalar);
				if (mEnvValue < ENV_VALUE_LOW || mReleaseIncr == 0.)
				{
					mStage    = EStage::kIdle;
					mEnvValue = 0.;

					if (mEndReleaseFunc)
						mEndReleaseFunc();
				}
				result = mEnvValue * mReleaseLevel;
				break;
			case EStage::kReleasedToRetrigger:
				mEnvValue -= mRetriggerReleaseIncr;
				if (mEnvValue < ENV_VALUE_LOW)
				{
					mStage        = EStage::kAttack;
					mLevel        = mNewStartLevel;
					mEnvValue     = 0.;
					mPrevResult   = 0.;
					mReleaseLevel = 0.;

					if (mResetFunc)
						mResetFunc();
				}
				result = mEnvValue * mReleaseLevel;
				break;
			case EStage::kReleasedToEndEarly:
				mEnvValue -= mEarlyReleaseIncr;
				if (mEnvValue < ENV_VALUE_LOW)
				{
					mStage        = EStage::kIdle;
					mLevel        = 0.;
					mEnvValue     = 0.;
					mPrevResult   = 0.;
					mReleaseLevel = 0.;
					if (mEndReleaseFunc)
						mEndReleaseFunc();
				}
				result = mEnvValue * mReleaseLevel;
				break;
			default:
				result = mEnvValue;
				break;
		}

		mPrevResult = result;
		mPrevOutput = (result * mLevel);
		return mPrevOutput;
	}

 private:
	constexpr TFLOAT CalcIncrFromTimeLinear(TFLOAT timeMS, TFLOAT sr) const
	{
		if (timeMS <= 0.)
			return 0.;
		else
			return (1 / sr) / (timeMS / 1000);
	}

	constexpr TFLOAT CalcIncrFromTimeExp(TFLOAT timeMS, TFLOAT sr) const
	{
		TFLOAT r;

		if (timeMS <= 0.0f)
			return 0.f;
		else
		{
			r = -std::expm1(1000.f * std::log(0.001f) / (sr * timeMS));
			if (!(r < 1.0f))
				r = 1.0f;

			return r;
		}
	}
};

END_IPLUG_NAMESPACE
