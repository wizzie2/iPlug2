
#include "IPlug.h"


namespace iplug
{
#if PLATFORM_WINDOWS
	const int GetScaleForHWND(const HWND hWnd, const bool useCachedResult)
	{
		static int CachedScale = 0;
		if (useCachedResult == false || CachedScale == 0)
			CachedScale = math::IntegralDivide(::GetDpiForWindow(hWnd), USER_DEFAULT_SCREEN_DPI);
		return CachedScale;
	}
#endif

}  // namespace iplug
