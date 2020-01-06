#pragma once
#include "Shader.h"

nativenamespace{

	class ShaderForScreenLines : public Shader
{
public:
	ShaderForScreenLines()
	{
		vertexString = lineVertexShader;
		fragmentString = lineFragmentShader;
		TransformID = -1;
		LineColour1 = -1;
		LineColour2 = -1;
		LinePointA = -1;
		LinePointB = -1;
		id = -1;
	}

	const char* lineVertexShader = R"(#version 420

			uniform vec2 pointA;
			uniform vec2 pointB;

			uniform vec4 pA;
			uniform vec4 pB;

			uniform vec4 col1;
			uniform vec4 col2;
			uniform mat4 transform; //full projection, not just model transform

			out fData
			{
				vec4 color;
			}frag;

			void main()
			{
				vec4 vec;
				if(gl_VertexID == 1)
				{
					vec = pA;
					frag.color = col2;
				}else
				{
					vec = pB;
					frag.color = col1;
				}
				gl_Position = vec;
			} 
			)";

	const char * lineFragmentShader = R"(
			#version 420

			layout(location=0) out vec4 outcolour;

			in fData
			{
				vec4 color;
			}frag;

			void main()
			{
				outcolour = frag.color;
			}

			//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
			)";

	//Adding line thickness
	//https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader
	const char* lineGeometryShader = R"(
			#version 420

			layout(triangles) in;
			layout(line_strip, max_vertices = 3) out;

			out fData
			{
				vec4 color;
			}frag;

			uniform vec4 PointA;
			uniform vec4 PointB;
			uniform vec4 col1;
			uniform vec4 col2;
			uniform mat4 transform;

			void main() 
			{
			  for(int i = 0; i < 2; i++)
			  { 
				if(i==0)
					frag.color = col2;  //flip final colours because line is draw from B to A
				else if(i==1)
					frag.color = col1;
				
				gl_Position = transform*PointA;

				EmitVertex();
				gl_Position = transform*PointB;

				EmitVertex();
			  }
			  EndPrimitive();
			}

			//for drawing lines in clip space
			//http://stackoverflow.com/questions/12758192/wide-lines-in-geometry-shader-behaves-oddly
			)";

	int TransformID;
	int LinePointA;
	int LinePointB;
	int LineColour1;
	int LineColour2;

	/*
	void DrawPoint(glm::vec3 point, glm::vec4 col,float scale,glm::mat4 transform, glm::mat4 projection)
	{
		float length = (1.f *scale);
		glm::vec3 up = glm::vec3(0.f, length, 0.f);
		glm::vec3 right = glm::vec3(length, 0.f, 0.f);
		glm::vec3 forward = glm::vec3(0.f, 0.f, length);

		DrawLine(point - up, point + up, col, col, 1.f, projection*transform, true);
		DrawLine(point - right, point + right, col, col, 1.f, projection*transform, true);
		DrawLine(point - forward, point + forward, col, col, 1.f, projection*transform, true);
	}

	void DrawOrigin(float scale, const glm::mat4& transform, const glm::mat4& projection)
	{
		DrawAxis(vec3(0.f), scale, transform, projection);
	}
	void DrawOrigin(float scale, const glm::mat4& projection)
	{
		DrawAxis(vec3(0.f), scale, mat4(1.f), projection);
	}

	void DrawPoint(glm::vec3 point, glm::vec4 col, float scale, glm::mat4 projection)
	{
		float length = (1.f *scale);
		glm::vec3 up = glm::vec3(0.f, length, 0.f);
		glm::vec3 right = glm::vec3(length, 0.f, 0.f);
		glm::vec3 forward = glm::vec3(0.f, 0.f, length);

		DrawLine(point - up, point + up, col, col, 1.f, projection, true);
		DrawLine(point - right, point + right, col, col, 1.f, projection, true);
		DrawLine(point - forward, point + forward, col, col, 1.f, projection, true);
	}

	void DrawAxis(glm::vec2 point, float scale,glm::mat4 transform, glm::mat4 projection)
	{
		glm::vec3 up = glm::vec3(0.f, 1.f*scale, 0.f);
		glm::vec3 right = glm::vec3(1.f*scale, 0.f, 0.f);
		glm::vec3 forward = glm::vec3(0.f, 0.f, 1.f*scale);

		DrawLine(point, point + up, glm::vec4(0.f, 1.f, 0.f, 1.f), glm::vec4(0.f, 1.f, 0.f, 1.f), 1.f, projection*transform, true);
		DrawLine(point, point + right, glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec4(1.f, 0.f, 0.f, 1.f), 1.f, projection*transform, true);
		DrawLine(point, point + forward, glm::vec4(0.f,0.f, 1.f, 1.f), glm::vec4(0.f, 0.f, 1.f, 1.f), 1.f, projection*transform, true);
	}
	*/
	void DrawLine(const glm::vec2& point1, const glm::vec2& point2, const glm::vec4& col1, const glm::mat4& projectionMatrix)
	{
		DrawLine(point1, point2, col1, col1, 1.f, projectionMatrix, true);
	}

	void DrawLine(const glm::vec2& point1, const glm::vec2& point2, const glm::vec4& col1, const glm::vec4& col2, const glm::mat4& projectionMatrix)
	{
		DrawLine(point1, point2, col1, col2, 1.f, projectionMatrix, true);
	}

	void DrawLine(const glm::vec2& point1, const glm::vec2& point2, const glm::vec4& col, const glm::vec4& col2, float width, const glm::mat4& projectionMatrix, bool drawOnTop)
	{

		checkGLError("pre Line shader draw check");

		SetUniform2f("pointA", point1);
		SetUniform2f("pointB", point2);

		glm::mat4 trans = projectionMatrix;
		SetUniform4x4("transform", trans);

		SetUniform4f("col1", col);
		SetUniform4f("col2", col2);

		glDrawArrays(GL_LINE_STRIP, 0, 2);

		checkGLError("Line shader check");
	}

	void DrawLine(const glm::vec4& point1, const glm::vec4& point2, const glm::vec4& col, const glm::mat4& projectionMatrix)
	{
		DrawLine(point1, point2, col, col, projectionMatrix);
	}

	void DrawLine(const glm::vec4& point1, const glm::vec4& point2, const glm::vec4& col, const glm::vec4& col2, const glm::mat4& projectionMatrix)
	{

		checkGLError("pre Line shader draw check");

		//SetUniform2f("pointA", point1);
		//SetUniform2f("pointB", point2);

		SetUniform4f("pA", point1);
		SetUniform4f("pB", point2);

		glm::mat4 trans = projectionMatrix;
		SetUniform4x4("transform", trans);

		SetUniform4f("col1", col);
		SetUniform4f("col2", col2);

		glDrawArrays(GL_LINE_STRIP, 0, 2);

		checkGLError("Line shader check");
	}

	/*
	void DrawGridQuick(const vec4& col, int cellCount, float gridSize, const mat4& proj)
	{
		float xsize, zsize = 0.f;
		int count = cellCount + 1;
		int halfCount = (float)count * 0.5f;
		int minCount = -halfCount - 1;
		float centreSize = (count * gridSize)*0.5f;
		vec3 centre = vec3(centreSize, 0, centreSize);

		glEnable(GL_LINE_SMOOTH);

		for (int z = minCount; z < halfCount; z++)
		{
			xsize = 0.f;
			for (int x = minCount; x < halfCount; x++)
			{
				int localX = abs(x + 1);
				float alphaX = ((float)(cellCount - localX)) / (float)cellCount;

				int localZ = abs(z + 1);
				float alphaZ = ((float)(cellCount - localZ)) / (float)cellCount;

				vec3 a1 = vec3(xsize, 0, 0) - centre;
				vec3 a2 = vec3(xsize, 0, zsize) - centre;
				DrawLine(a1, a2, col, proj);

				vec3 b1 = vec3(0, 0, zsize) - centre;
				vec3 b2 = vec3(xsize, 0, zsize) - centre;
				DrawLine(b1, b2, col, proj);

				xsize += gridSize;
			}

			zsize += gridSize;
		}
	}

	void DrawAABB(const glm::vec3& min,const glm::vec3& max, const glm::vec4& col, const glm::mat4& projection)
	{
		vec3 l_0 = min;
		vec3 l_1 = vec3(max.x, min.y, min.z);
		vec3 l_2 = vec3(max.x, min.y, max.z);
		vec3 l_3 = vec3(min.x, min.y, max.z);

		vec3 y_offset = vec3(0.f, max.y,0.f);

		vec3 u_0 = l_0 + y_offset;
		vec3 u_1 = l_1 + y_offset;
		vec3 u_2 = l_2 + y_offset;
		vec3 u_3 = l_3 + y_offset;

		DrawLine(l_0, l_1,col, projection);
		DrawLine(l_1, l_2,col, projection);
		DrawLine(l_2, l_3,col, projection);
		DrawLine(l_3, l_0,col, projection);

		DrawLine(u_0, u_1,col, projection);
		DrawLine(u_1, u_2,col, projection);
		DrawLine(u_2, u_3,col, projection);
		DrawLine(u_3, u_0,col, projection);

		DrawLine(u_0, l_0,col, projection);
		DrawLine(u_1, l_1,col, projection);
		DrawLine(u_2, l_2,col, projection);
		DrawLine(u_3, l_3,col, projection);
	}
	*/
};
}


