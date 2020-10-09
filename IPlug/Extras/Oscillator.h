/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

BEGIN_IPLUG_NAMESPACE

template <typename T = tfloat>
struct IOscillator
{
	IOscillator(T startPhase = 0., T startFreq = 1.) : mStartPhase(startPhase)
	{
		SetFreqCPS(startFreq);
	}

	//	inline T Process(T freqHz) = 0;

	constexpr void SetFreqCPS(T freqHz)
	{
		mPhaseIncr = mSampleRateReciprocal * freqHz;
	}

	constexpr void SetSampleRate(T sampleRate)
	{
		mSampleRateReciprocal = (1 / sampleRate);
	}

	constexpr void Reset()
	{
		mPhase = mStartPhase;
	}

	constexpr void SetPhase(T phase)
	{
		mPhase = phase;
	}

 protected:
	T mSampleRateReciprocal = 1 / 44100.;
	T mStartPhase           = 0;
	T mPhase                = 0;                      // float phase (goes between 0. and 1.)
	T mPhaseIncr            = mSampleRateReciprocal;  // how much to add to the phase on each T
};


template <typename T = tfloat>
struct SinOscillator : public IOscillator<T>
{
	SinOscillator(T startPhase = 0., T startFreq = 1.) : IOscillator<T>(startPhase, startFreq) {}

	constexpr T Process() const
	{
		this->mPhase = this->mPhase + this->mPhaseIncr;
		return std::sin(this->mPhase * math::constants::pi_v<T> * 2);
	}

	constexpr T Process(T freqHz)
	{
		this->SetFreqCPS(freqHz);
		this->mPhase = this->mPhase + this->mPhaseIncr;
		return std::sin(this->mPhase * math::constants::pi_v<T> * 2);
	}
};

/*
 FastSinOscillator - fast sinusoidal oscillator / table look up, based on an approach and code used by Miller Puckette
 in Pure Data, originally by Robert Höldrich

 From some correspondence with M.S.P...

 The basic idea is this: if you have a double precision floating point number
 between -2^19 (-524288)  and +2^19 (+524288), and you want the fractional part as an
 integer, proceed as follows:

 Add 3*2^19 (1572864) to it.  The sum will be between 2^20 (1048576) and 2^21 (2097152) and so, in the usual
 double precision format, the units bit will be the LSB of the higher-order
 32 bits, and the fractional part will be neatly represented as an unsigned
 fixed-point number between 0 and 2^32-1.

 You can then use a C union to read these lower 32 bits as an integer.  Or, if
 you want to access then as a floating-point number without having to use
 fix-to-float conversion, just bash the upper 32 bits to their original
 state for representing 3*2^19.  So far this is what's going on in the wrap~
 and phasor~ objects in Pd.

 The objects that do interpolating table lookup (e.g., cos~) take this one step
 further.  Here you end up with a number (now supposing it to be between
 -2^10 and +2^10).  Multiply by 2^9 (the size of the table) so that the
 9-bit table address is in the 9 lowest bits of the upper 32 bit 'word', and
 the fractional part is in the lower 32 bits as before.  Grab the address as
 an integer (masking to get just the 9 bits we want).

 The you need to re-convert the fractional address into floating-point format
 so you can use it to interpolate.  Just bash the upper 32 bits with the
 original value for 3*2^19, and the result will be a 64-bit floating-point number
 between 3*2^19 and 3*2^19+1.  Subtracting 3*2^19 from this gives us a floating-
 point representation of the fractional part.
 */

// TODO: Benchmark this...

template <class T = tfloat>
class FastSinOscillator : public IOscillator<T>
{
	static constexpr double UNITBIT32 = 1572864.; /* 3*2^19; bit 32 has place value 1 */
	static constexpr auto HIOFFSET    = 1;
	static constexpr auto LOWOFFSET   = 0;

	union tabfudge
	{
		double d;
		int i[2];
	};

 public:
	FastSinOscillator(T startPhase = 0., T startFreq = 1.) : IOscillator<T>(startPhase * tableSizeM1, startFreq) {}

	// todo rewrite this
	constexpr T Process() const
	{
		T output = 0.;
		ProcessBlock(&output, 1);

		return output;
	}

	constexpr T Process(T freqCPS)
	{
		IOscillator<T>::SetFreqCPS(freqCPS);

		T output = 0.;
		ProcessBlock(&output, 1);

		return output;
	}

	constexpr void ProcessBlock(T* pOutput, int nFrames)
	{
		T phase           = this->mPhase + static_cast<T>(UNITBIT32);
		const T phaseIncr = this->mPhaseIncr * tableSize;

		tabfudge tf {UNITBIT32};

		const int normhipart = tf.i[HIOFFSET];

		for (auto s = 0; s < nFrames; s++)
		{
			tf.d = phase;
			phase += phaseIncr;
			const int addr = tf.i[HIOFFSET] & tableSizeM1;  // Obtain the integer portion
			tf.i[HIOFFSET] = normhipart;                    // Force the double to wrap.
			const T frac   = static_cast<T>(tf.d - UNITBIT32);
			const T f1     = static_cast<T>(mLUT[addr]);
			const T f2     = static_cast<T>(mLUT[addr + 1]);
			mLastOutput = pOutput[s] = T(f1 + frac * (f2 - f1));
		}

		// Restore mPhase
		tf.d                  = UNITBIT32 * tableSize;
		const int normhipart2 = tf.i[HIOFFSET];

		// Remove the offset we introduced at the start of UNITBIT32.
		tf.d           = phase + (UNITBIT32 * tableSize - UNITBIT32);
		tf.i[HIOFFSET] = normhipart2;
		this->mPhase   = static_cast<T>(tf.d - UNITBIT32 * tableSize);
	}

	static constexpr int tableSize   = 1 << 9;         // 2^9
	static constexpr int tableSizeM1 = tableSize - 1;  // 2^9 -1  (assumes tableSize is a Pow2 value)

	static const std::array<double, tableSize + 1> mLUT;

	T mLastOutput = 0.;
};

template <class T>
const std::array<double, FastSinOscillator<T>::tableSize + 1> FastSinOscillator<T>::mLUT = [] {
	alignas(16) static std::array<double, tableSize + 1> array;

	for (int i = 0; i < tableSize; ++i)
	{
		double d = sin(i * math::constants::pi_v<double> * 2 / tableSize);
		array[i] = math::IsNearlyZero(d) ? 0.0 : d;
	}
	array[tableSize] = array[0];
	return array;
}();

END_IPLUG_NAMESPACE
