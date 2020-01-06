#pragma once
#include "Shader.h"
#include "Bone.h"

nativenamespace{

	class ShaderForBones : public Shader
	{
	public:
		ShaderForBones()
		{
			vertexString = boneVertexShader;
			geometryString = boneGeometryShader;
			fragmentString = boneFragmentShader;
			TransformID = -1;
			LineColour1 = -1;
			LineColour2 = -1;
			LinePointA = -1;
			LinePointB = -1;
			id = -1;
		}

		const char* boneVertexShader = R"(#version 420

		uniform vec3 pointA;
		uniform vec4 colour;
		uniform mat4 look;

		out vData
		{
			vec3 point;
			vec4 colour;
		}vertex;

		void main()
		{
			vertex.point = vec3(inverse(look)*vec4(pointA,1.0));
			vertex.colour = colour;
		} 
		)";

		const char* boneGeometryShader = R"(
		#version 420

		layout(points) in;
		layout(triangle_strip, max_vertices = 36) out;

		in vData
		{
			vec3 point;
			vec4 colour;
		}vertex[];

		out fData
		{
			vec4 color;
			vec4 pos;
		}frag;

		uniform int boneID;
		uniform int meshID;
		uniform mat4 projection;
		uniform mat4 model;
		uniform mat4 look;
		bool showWinding;

		void EmitPoint(vec3 p,vec4 colour)
		{
			frag.color = colour;
			frag.pos = projection*model*look*vec4(p,1.0);
			gl_Position = frag.pos;
			EmitVertex();
		}

		void EmitTri(vec3 a, vec3 b, vec3 c,vec4 colour)
		{
			if(!showWinding)
			{
				EmitPoint(a,colour);
				EmitPoint(b,colour);
				EmitPoint(c,colour);
			}else{
				EmitPoint(c,colour - vec4(1.0,1.0,1.0,0.0));
				EmitPoint(b,colour);
				EmitPoint(a,colour + vec4(1.0,1.0,1.0,0.0));
			}
			EndPrimitive();
		}

		void EmitBone(vec4 colour)
		{
			vec3 back = vec3(0);
			vec3 tip = vertex[0].point;
			float _l = 1.0;
			vec3 right  = vec3(_l, 0, -1);
			vec3 top    = vec3(0, _l, -1);
			vec3 bottom = vec3(0,-_l, -1);
			vec3 left   = vec3(-_l,0, -1);
			
			//top right
			EmitTri(tip, right, top,colour);

			//bottom right
			EmitTri(tip, bottom, right,colour);

			//top left
			EmitTri(tip, top, left,colour);

			//bottom left
			EmitTri(left, bottom,tip,colour);

			//back right top
			EmitTri(top, right, back,colour);

			//back left top
			EmitTri(back, left, top,colour);

			//back right bottom
			EmitTri(back, right, bottom,colour);

			//back left bottom
			EmitTri(bottom, left, back,colour);
		}

		void main() 
		{
			showWinding = true;
			vec4 col = vertex[0].colour;
			EmitBone(col);
		}

		//for drawing lines in clip space
		//http://stackoverflow.com/questions/12758192/wide-lines-in-geometry-shader-behaves-oddly
		)";

		const char * boneFragmentShader = R"(
		#version 420

		layout (location = 0) out vec4 outcolour;
		layout (location = 1) out vec4 outposition;
		layout (location = 2) out uvec4 outid;

		in fData
		{
			vec4 color;
			vec4 pos;
		}frag;

		uniform int boneID;
		uniform int meshID;

		void main()
		{
			vec4 col = frag.color;
			if(!gl_FrontFacing)
				col = vec4(1,0,0,1);
			outcolour = col;
			outposition = frag.pos;
			outid = uvec4(meshID,boneID,0,0);
		}

		//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
		)";

		int TransformID;
		int LinePointA;
		int LinePointB;
		int LineColour1;
		int LineColour2;

		void DrawBone(const Bone* bone,const mat4& projectionMatrix, const mat4& transform = mat4(1.f),const vec4& col = COLOUR_GREY, float scale = 10.f)
		{
			if (bone->parent == NULL)
				return;
			vec3 p1 = bone->GetWorldPosition();
			vec3 p2 = bone->parent->GetWorldPosition();
			DrawBone(p1, p2, col, scale, transform, projectionMatrix);
		}

		void DrawBone(const vec3& point1, const vec3& point2, const vec4& col, const mat4& projectionMatrix)
		{
			DrawBone(point1, point2, col, 10.f,mat4(1.f), projectionMatrix);
		}

		void DrawBone(const vec3& point1, const vec3& point2, const vec4& col,float scale,const mat4& transform, const mat4& projectionMatrix)
		{
			checkGLError("pre Line shader draw check");

			vec3 c = glm::cross(point2 - point1,vec3(0,1,0));
			//use the distance as scale
			float dist = glm::distance(point1, point2);
			//construct world matrix by inverting the look at
			mat4 look = glm::scale(glm::inverse(glm::lookAt(point2, point1, c)), vec3(dist/scale));
			//convert our world points to the bones local space. 
			//the shader will convert back to world space.  back point becomes origin (000) in local space
			//so technically we only need to send the tip to the shader
			//mat4 l2 = glm::inverse(look);
			//vec4 p1 = l2*vec4(point1, 1);
			//vec4 p2 = l2*vec4(point2, 1);
			SetUniform3f("pointA", point1);
			//SetUniform3f("pointB", vec3(p2));

			SetUniform4x4("projection", projectionMatrix);
			SetUniform4x4("model", transform);
			SetUniform4x4("look", look);
			SetUniform4f("colour", col);

			glDrawArrays(GL_POINTS, 0, 1);

			checkGLError("Line shader check");
		}
	};
}


