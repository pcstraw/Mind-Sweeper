#pragma once
#include "NativeHeader.h"

nativenamespace {

	class Utils {
	public:
		static std::vector<float> GetFloatArray(const std::vector<glm::vec2>& points)
		{
			std::vector<float> data;
			for (const glm::vec2& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
			}
			return data;
		}

		static std::vector<float> GetFloatArray(const std::vector<glm::vec3>& points)
		{
			std::vector<float> data;
			for (const glm::vec3& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
			}
			return data;
		}
		static std::vector<float> GetFloatArrayPadded(const std::vector<glm::vec3>& points)
		{
			std::vector<float> data;
			for (const glm::vec3& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
				data.push_back(0.0);
			}
			return data;
		}

		static std::vector<float> GetFloatArray(const std::vector<glm::vec4>& points)
		{
			std::vector<float> data;
			for (const glm::vec4& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
				data.push_back(v.w);
			}
			return data;
		}

		//ints
		static std::vector<int> GetIntArray(const std::vector<glm::ivec2>& points)
		{
			std::vector<int> data;
			for (const glm::ivec2& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
			}
			return data;
		}

		static std::vector<int> GetIntArray(const std::vector<glm::ivec3>& points)
		{
			std::vector<int> data;
			for (const glm::ivec3& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
			}
			return data;
		}

		static std::vector<int> GetIntArrayPadded(const std::vector<glm::ivec3>& points)
		{
			std::vector<int> data;
			for (const glm::ivec3& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
				data.push_back(0);
			}
			return data;
		}

		static std::vector<int> GetIntArray(const std::vector<glm::ivec4>& points)
		{
			std::vector<int> data;
			for (const glm::ivec4& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
				data.push_back(v.w);
			}
			return data;
		}

		static std::vector<unsigned int> GetUIntArray(const std::vector<glm::uvec2>& points)
		{
			std::vector<unsigned int> data;
			for (const glm::uvec2& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
			}
			return data;
		}

		static std::vector<unsigned int> GetUIntArrayPadded(const std::vector<glm::uvec3>& points)
		{
			std::vector<unsigned int> data;
			for (const glm::uvec3& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
				data.push_back(0);
			}
			return data;
		}

		static std::vector<unsigned int> GetUIntArray(const std::vector<glm::uvec3>& points)
		{
			std::vector<unsigned int> data;
			for (const glm::uvec3& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
			}
			return data;
		}

		static std::vector<unsigned int> GetUIntArray(const std::vector<glm::ivec3>& points)
		{
			std::vector<unsigned int> data;
			for (const glm::ivec3& v : points)
			{
				data.push_back((unsigned int)v.x);
				data.push_back((unsigned int)v.y);
				data.push_back((unsigned int)v.z);
			}
			return data;
		}

		static std::vector<unsigned int> GetUIntArray(const std::vector<glm::uvec4>& points)
		{
			std::vector<unsigned int> data;
			for (const glm::uvec4& v : points)
			{
				data.push_back(v.x);
				data.push_back(v.y);
				data.push_back(v.z);
				data.push_back(v.w);
			}
			return data;
		}
	};
}
