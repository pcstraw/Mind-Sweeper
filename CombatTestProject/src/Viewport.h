#pragma once
#include "NativeHeader.h"
#define WIDTH           Viewport::window->width
#define HEIGHT          Viewport::window->height

nativenamespace{

	class Viewport
	{
	public:

		Viewport() {};

		Viewport(int Width, int Height)
		{
			width = Width;
			height = Height;
		}

		Viewport(const ivec2& view)
		{
			width = view.x;
			height = view.y;
		}

		void Size(int viewportWidth, int viewportHeight)
		{
			width = viewportWidth;
			height = viewportHeight;

			for (std::function<void(int, int)> f : callbacks)
				f(width, height);
		}

		int width = defaultWIDTH;
		int height = defaultHEIGHT;
		bool changed = false;
		std::vector<std::function<void(int, int)>> callbacks;

		static Viewport* window;
	};
}