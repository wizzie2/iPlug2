#pragma once

/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/


#include "PAL/Generic/Generic_System.h"

namespace iplug
{
	class System : public generic::System
	{
		class cpu_id;  // fwd

	 public:
	 private:
		static const cpu_id m_cpu_id;
	};
}  // namespace iplug

