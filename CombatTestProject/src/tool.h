#pragma once
#include "NativeHeader.h"
//#include "ManagedHeader.h"

namespace Native
{
	class tool
	{
	public:

		static float RoundToSingleDecimal(float num)
		{
			//todo:  convert to native
			return truncf(num * 10.0) / 10.0;
			//return round(num / 2) * 2;
			//return (float)Math::Round(num, 1, MidpointRounding::ToEven);
		}

		static glm::vec4 Round(const glm::vec4 vec)
		{
			glm::vec4 col;
			col.r = RoundToSingleDecimal(vec.r);
			col.g = RoundToSingleDecimal(vec.g);
			col.b = RoundToSingleDecimal(vec.b);
			col.a = RoundToSingleDecimal(vec.a);
			return col;
		}

		template <typename T>
		static void DebugObject(const char* s, const T& object)
		{
			cout << "\n-> > " << s << ": " << object << "<-" << endl;
		}

		static void DebugMatrix(const char* text, const glm::mat4& mat)
		{
			cout << "\n::DebugMatrix: " << endl;
			cout << "[ " << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << " " << mat[0][3] << " ]" << endl;
			cout << "[ " << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << " " << mat[1][3] << " ]" << endl;
			cout << "[ " << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << " " << mat[2][3] << " ]" << endl;
			cout << "[ " << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << " ]" << endl;
		}

		static void  DebugString(const char* s)
		{
			cout << "\n-> > " << s << " <-" << endl;
		}

		static void Print(glm::vec3 vec3)
		{
			cout << " Debug glm::vec3: " << endl;
			cout << "X: " << vec3.x << endl;
			cout << "Y: " << vec3.y << endl;
			cout << "Z: " << vec3.z << endl;
			cout << endl;
		}

		static void Print(glm::vec3 vec3, const char * text)
		{
			cout << " Debug glm::vec3: " << text << endl;
			cout << "X: " << vec3.x << endl;
			cout << "Y: " << vec3.y << endl;
			cout << "Z: " << vec3.z << endl;
			cout << endl;
		}

		static void Print(glm::vec4 vec4)
		{
			cout << " Debug glm::vec4: " << endl;
			cout << "X: " << vec4.x << endl;
			cout << "Y: " << vec4.y << endl;
			cout << "Z: " << vec4.z << endl;
			cout << "W: " << vec4.w << endl;
			cout << endl;
		}

		static void Print(glm::vec4 vec4, const char * text)
		{
			cout << " Debug glm::vec4: " << text << endl;
			cout << "X: " << vec4.x << endl;
			cout << "Y: " << vec4.y << endl;
			cout << "Z: " << vec4.z << endl;
			cout << "W: " << vec4.w << endl;
			cout << endl;
		}
	};
}

