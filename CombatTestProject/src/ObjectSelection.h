#pragma once
#include "NativeHeader.h"
#include "tool.h"

nativenamespace{

	class ObjectSelection
	{
	public:
		ObjectSelection() 
		{
			R = 0.f;
			G = 0.f;
			B = 0.f;
			A = 1.f;
			increment = 0.1f;
		};

		std::vector<glm::vec4> colourIDs;
		float R;
		float G;
		float B;
		float A;
		float increment;
		static ObjectSelection manager;

		bool IsID(const glm::vec4& col1,const glm::vec4& col2)
		{
			if (tool::Round(col1) == tool::Round(col2))
				return true;
			return false;
		}

		bool ContainsID(const glm::vec4& col)
		{
			for(glm::vec4& c : colourIDs)
			{
				if (IsID(c, col))
					return true;
			}
			return false;
		}

		int CreateColourID()
		{
			int i = colourIDs.size();
			int r = (i & 0x000000FF) >> 0;
			int g = (i & 0x0000FF00) >> 8;
			int b = (i & 0x00FF0000) >> 16;
			//int a = (i & 0xFF000000) >> 24;
			glm::vec4 raw = glm::vec4(r / 255.f,g / 255.f,b / 255.f,1.f);
			colourIDs.push_back(raw);
			return i;
		}

		static int GenerateID()
		{
			return manager.CreateColourID();
		}
	};
}