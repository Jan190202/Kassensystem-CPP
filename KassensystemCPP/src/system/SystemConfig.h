#pragma once

namespace systemConfig
{
	void setUTF8Encoding();

	constexpr bool isDebug()
	{
		#ifdef _DEBUG
			return true;
		#else
			return false;
		#endif
	}
}