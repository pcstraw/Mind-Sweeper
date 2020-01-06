#pragma once
#include "NativeHeader.h"
#include "Shader.h"
#include "ShaderFactory.h"
#include "Framebuffer.h"

nativenamespace{

	class ShaderDeferedLighting : public Shader
	{

	public:

		ShaderDeferedLighting()
		{
			clearColour = vec4(0.f);
		}

		Framebuffer gbuffer;
		vec3 testLight;
		vec4 clearColour;

		void BuildAndCompile(int width, int height)
		{
			CreateFramebuffer(width, height);
			Shader s = ShaderFactory::GetDeferedLighting();
			vertexString = s.vertexString;
			fragmentString = s.fragmentString;
			Compile("Defered Shader");
		}

		void Begin()
		{
			gbuffer.clearColour = clearColour;
			gbuffer.Begin();
		}

		void End()
		{
			gbuffer.End();
		}

		void DrawLighting(Camera* camera)
		{
			Select();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, gbuffer.textureArrayID);
		
			SetUniform1i("textureArray", 0);
			SetUniform2f("screensize", vec2(camera->width, camera->height));
			SetUniform3f("eyepos", camera->position);
			SetUniform4x4("inverseprojection", camera->inverseProjectionMatrix);
			SetUniform4x4("view", camera->viewMatrix);
			SetUniform3f("lightpos", testLight);

			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
			BlitDpethToDefaultBuffer(camera->width, camera->height);
		}

		void BlitToDefault(int width, int height, GLenum mask, GLenum filter)
		{
			gbuffer.BlitToDefault(width, height, mask, filter);
		}

		void BlitDpethToDefaultBuffer(int Width, int Height)
		{
			gbuffer.BlitDpethToDefaultBuffer(Width, Height);
		}

		void ClearTextureArray(const vec4& col)
		{
			gbuffer.ClearTextureArray(col);
		}

		bool CreateFramebuffer(int WindowWidth, int WindowHeight)
		{
			return gbuffer.CreateDeferedFramebuffer(WindowWidth, WindowHeight, 4);
		}
	};
}

