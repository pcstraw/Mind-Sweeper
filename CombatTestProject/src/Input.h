#pragma once
#include "NativeHeader.h"
#include "Mouse.h"
#include "Keyboard.h"

nativenamespace
{
	class Input
	{
	public:

		static Mouse mouse;
		static Keyboard keyboard;
	};
}