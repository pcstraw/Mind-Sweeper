#pragma once
#include "NativeHeader.h"

nativenamespace{

	class Timestep
	{
	public:

		static float delta;
		static float GetMilliSeconds() { return delta * 1000.f; }

	};
}

