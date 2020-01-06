#pragma once
#define nativenamespace namespace Native

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "Windows.h"
#define GLEW_STATIC
#include <glew.h>
#define GLFW_INCLUDE_GLEXT
#include <GLFW\glfw3.h>
//#include "GLUT\freeglut.h" //used for wglcontext calls and gluGetErrorString
#include <glm\glm.hpp>
#include <glm\mat4x4.hpp>
#include <glm\mat3x3.hpp>
#include <glm\trigonometric.hpp>
#include <glm\gtx\projection.hpp>
#include <glm\matrix.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <iostream>
#include "SDKDDKVer.h"  //<- defs for latest window being used
#include <stdio.h>
#include <tchar.h>
#include <shellapi.h>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
using std::cout;
//using std::endl;
using std::cerr;
using std::ifstream;
#include <fstream>
#include <cstring>
#include <sstream>
#include <iterator>
#include <string>
#include <math.h>
#include <fstream>
#include <limits>
#include <functional>
#include <algorithm>
#define nativenamespace namespace Native

#define endl "\n"
using string = std::string;
using quat = glm::quat;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;
using ivec4 = glm::ivec4;
using uvec2 = glm::uvec2;
using uvec3 = glm::uvec3;
using uvec4 = glm::uvec4;
using dvec2 = glm::dvec2;
using dvec3 = glm::dvec3;
using dvec4 = glm::dvec4;
using mat2 = glm::mat2;
using mat3 = glm::mat3;
using mat4 = glm::mat4;

#define gui ImGui

# define math_pi           3.14159265358979323846f
# define math_deg  math_pi/180.f
# define math_to_degrees 57.295779
# define math_to_radians math_pi/180.f
# define NULL 0
# define time_step  1.f/60.f
#define defaultWIDTH           1080
#define defaultHEIGHT          720
#define WINDOW_TILE "Combat Test"
#define SHADER_GEOMETRY_PATH string("shaders\\Geometry\\")
#define SHADER_LIGHTING_PATH string("shaders\\Lighting\\")
#define SHADER_TEXTURE_PATH string("shaders\\Texture\\")

#define ASSET_TEXTURE_PATH string("Assets\\Textures\\")

#define to_string std::to_string
#define float_max std::numeric_limits<float>::max()

#define getName(name) GetVariableName(#name)
/*
static const char* GetVariableName(char *name) {
	return name;
}
*/

static const char* GetVariableName(const char *name) {
	return name;
}

static string FloatToString(float f)
{
	std::ostringstream ss;
	ss << f;
	std::string s(ss.str());
	return s;
}


#define COLOUR_RED glm::vec4(1,0,0,1)
#define COLOUR_BLUE glm::vec4(0,0,1,1)
#define COLOUR_LIGHT_BLUE glm::vec4(0.1f,0.8f,1.f,1.f)
#define COLOUR_GREEN glm::vec4(0,1,0,1)
#define COLOUR_DARK_GREEN glm::vec4(0.f,0.2f,0.f,1.f)
#define COLOUR_YELLOW glm::vec4(1,1,0,1)
#define COLOUR_PURPLE glm::vec4(1,0,1,1)
#define COLOUR_WHITE glm::vec4(1,1,1,1)
#define COLOUR_NONE glm::vec4(0,0,0,0)
#define COLOUR_BLACK glm::vec4(0,0,0,1)
#define COLOUR_GREY glm::vec4(0.5,0.5,0.5,1)
#define COLOUR_DARK_GREY glm::vec4(0.2,0.2,0.2,1)
#define COLOUR_LIGHT_GREY glm::vec4(0.8,0.8,0.8,1)
#define COLOUR_VIOLET glm::vec4(0.5,0,1,1)

static int checkGLError(std::string text)
{
	int i = glGetError();

	if (i != 0)
	{
		cout << " " << text << "\n: OPENGL ERROR: " << i << endl;
		bool breakhere = true;
	}
	return i;
}

static void CheckGLSpecs(const char* text, GLenum e)
{
	int i;
	glGetIntegerv(e, &i);
	cout << ":: Check GL specs: \n:: " << text << i << endl;
}

static void ClearColour(glm::vec4 colour)
{
	glClearColor(colour.x, colour.y, colour.z, colour.w);
}

static void ClearColour(glm::vec3 colour)
{
	glClearColor(colour.x, colour.y, colour.z, 1.f);
}

static void ClearColour(glm::vec3 colour, float alpha)
{
	glClearColor(colour.x, colour.y, colour.z, alpha);
}

static void ClearColour(float colour)
{
	glClearColor(colour, colour, colour, 1.f);
}

static void ClearColourAndDepth(glm::vec4 colour)
{
	ClearColour(colour);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void ClearColourAndDepth(glm::vec3 colour, float alpha)
{
	ClearColour(colour, alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void ClearColourAndDepth(glm::vec3 colour)
{
	ClearColour(colour);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void ClearColourAndDepth(float colour)
{
	ClearColour(colour);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void ClearColourAndDepth()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void ClearColourOnly(const glm::vec4& colour)
{
	glClearColor(colour.x, colour.y, colour.z, colour.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void ClearDepthOnly()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

static int nextPowerOfTwo(int x) {
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

static bool glEnableDepth(bool enable)
{
	if (!enable)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		return enable;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	return enable;
}

static bool glEnableTransparency(bool enable)
{
	if (enable)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_BLEND);
	}
	return enable;
}

static void BindAsTexture2D(int bindingPoint,unsigned int textureID)
{
	glActiveTexture(GL_TEXTURE0 + bindingPoint);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static string glmGetString(const vec2& vec)
{
	string x = to_string(vec.x);
	string y = to_string(vec.y);

	return string("X.." + x + "..Y.." + y);
}


static string glmGetString(const vec3& vec)
{
	string x = to_string(vec.x);
	string y = to_string(vec.y);
	string z = to_string(vec.z);

	return string("X.." + x + "..Y.." + y + "..Z.." + z);
}

static string glmGetString(const vec4& vec)
{
	string x = to_string(vec.x);
	string y = to_string(vec.y);
	string z = to_string(vec.z);
	string w = to_string(vec.w);

	return string("X.." + x + "..Y.." + y + "..Z.." + z + "..W.." + w);
}

template<typename T>
struct enumStrings
{
	static char const* data[];
};

// This is a utility type.
// Created automatically. Should not be used directly.
template<typename T>
struct enumRefHolder
{
	T& enumVal;
	enumRefHolder(T& enumVal) : enumVal(enumVal) {}
};
template<typename T>
struct enumConstRefHolder
{
	T const& enumVal;
	enumConstRefHolder(T const& enumVal) : enumVal(enumVal) {}
};

// The next two functions do the actual work of reading/writing an
// enum as a string.
template<typename T>
std::ostream& operator<<(std::ostream& str, enumConstRefHolder<T> const& data)
{
	return str << enumStrings<T>::data[data.enumVal];
}

template<typename T>
std::istream& operator>>(std::istream& str, enumRefHolder<T> const& data)
{
	std::string value;
	str >> value;

	// These two can be made easier to read in C++11
	// using std::begin() and std::end()
	//  
	static auto begin = std::begin(enumStrings<T>::data);
	static auto end = std::end(enumStrings<T>::data);

	auto find = std::find(begin, end, value);
	if (find != end)
	{
		data.enumVal = static_cast<T>(std::distance(begin, find));
	}
	return str;
}


// This is the public interface:
// use the ability of function to deduce their template type without
// being explicitly told to create the correct type of enumRefHolder<T>
template<typename T>
enumConstRefHolder<T>  enumToString(T const& e) { return enumConstRefHolder<T>(e); }

template<typename T>
enumRefHolder<T>       enumFromString(T& e) { return enumRefHolder<T>(e); }