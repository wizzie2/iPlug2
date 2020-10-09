#pragma once

/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/

namespace iplug::generic
{
	struct MXCSRScope
	{
		MXCSRScope(uint32 MXCSR_Flags = 0) {}

	 protected:
		uint32 m_MXCSR = 0;
	};


	struct System
	{
		// Pre-defined cache line size
		static inline constexpr int CacheLineSize()
		{
			return +ECacheLineSize::Native;
		}

		// Current runtime endianness
		static inline const EEndian GetEndianness()
		{
			static volatile const union
			{
				uint16 i;
				uint8 c[2];
			} u = {0x0001};
			return u.c[0] == 0x01 ? EEndian::Little : EEndian::Big;
		}
	};

}  // namespace iplug::generic
