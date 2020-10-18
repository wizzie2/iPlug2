#pragma once

/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/

//-----------------------------------------------------------------------------
// Documentation for system.h

#ifdef DOXYGEN_GENERATING_OUTPUT
namespace iplug
{
	struct System
	{
		//! @retval int Pre-defined cache line size
		static inline constexpr int CacheLineSize();

		//! @retval #EEndian Current runtime endianness
		static inline const EEndian GetEndianness();
	};
}  // namespace iplug
#endif  // DOXYGEN_GENERATING_OUTPUT


//-----------------------------------------------------------------------------

#include "Generic/Generic_System.h"

#if __has_include(PLATFORM_HEADER(System.h))
	#include PLATFORM_HEADER(System.h)
#endif


//-----------------------------------------------------------------------------
// In System.h for now
namespace iplug
{
#if PLATFORM_INTRINSIC_X64

	struct MXCSRScope : generic::MXCSRScope
	{
		MXCSRScope(uint32 MXCSR_Flags)
		{
			m_MXCSR = _mm_getcsr();
			_mm_setcsr(m_MXCSR | MXCSR_Flags);
		}

		~MXCSRScope()
		{
			_mm_setcsr(m_MXCSR);
		}
	};

	#define SCOPED_NO_DENORMALS()        \
		MXCSRScope PREPROCESSOR_CONCAT4( \
			_MXCSRScope_, __COUNTER__, _, __LINE__(_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON))

#else

	#define SCOPED_NO_DENORMALS()
	using MXCSRScope = generic::MXCSRScope;

#endif
}  // namespace iplug
