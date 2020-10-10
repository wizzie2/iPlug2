/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

BEGIN_IPLUG_NAMESPACE

template <typename T, int NC = 1>
class LogParamSmooth
{
 private:
	T mA, mB;
	T mOutM1[NC];

 public:
	LogParamSmooth(T timeMs = 5., T initalValue = 0.)
	{
		for (auto i = 0; i < NC; i++)
		{
			mOutM1[i] = initalValue;
		}

		SetSmoothTime(timeMs);
	}

	// only works for NC = 1
	constexpr T Process(T input)
	{
		mOutM1[0] = (input * mB) + (mOutM1[0] * mA);
		return mOutM1[0];
	}

	constexpr void SetValue(T value)
	{
		for (auto i = 0; i < NC; i++)
		{
			mOutM1[i] = value;
		}
	}

	constexpr void SetValues(T values[NC])
	{
		for (auto i = 0; i < NC; i++)
		{
			mOutM1[i] = values[i];
		}
	}

	constexpr void SetSmoothTime(T timeMs, T sampleRate = Config::defaultSampleRate)
	{
		static constexpr T TWO_PI = 6.283185307179586476925286766559;

		if constexpr (type::IsFloat<T>)
			mA = expf(-TWO_PI / (timeMs * 0.001f * sampleRate));
		else
			mA = exp(-TWO_PI / (timeMs * 0.001 * sampleRate));
		mB = 1 - mA;
	}

	void ProcessBlock(T inputs[NC], T** outputs, int nFrames, int channelOffset = 0)
	{
		SCOPED_NO_DENORMALS();

		const T b = mB;
		const T a = mA;

		for (auto s = 0; s < nFrames; ++s)
		{
			for (auto c = 0; c < NC; c++)
			{
				T output                      = (inputs[channelOffset + c] * b) + (mOutM1[c] * a);
				mOutM1[c]                     = output;
				outputs[channelOffset + c][s] = output;
			}
		}
	}

} WDL_FIXALIGN;

END_IPLUG_NAMESPACE
