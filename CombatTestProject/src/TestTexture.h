#pragma once
#include "SceneBase.h"
#include "Texture2d.h"
#include "ShaderDeferedLighting.h"
#include "Renderer.h"
#include "Editor.h"

nativenamespace{

	class TestTexture : public SceneBase {

	public:

		Texture2d texture;
		Texture2d background;
		Shader texShader;
		unsigned int textureID;

		void OnLoad() override
		{
			texture = Texture2d::LoadFromFile(ASSET_TEXTURE_PATH + "quatre.png");
			background = Texture2d::LoadFromFile(ASSET_TEXTURE_PATH + "love.jpg");
			texShader.vertexString = Shader::LoadFile(SHADER_TEXTURE_PATH + "quad.vert");
			texShader.fragmentString = Shader::LoadFile(SHADER_TEXTURE_PATH + "quad.frag");

			texShader.Compile("Texture Shader");
		}

		void OnUpdate() override
		{
			Editor::MouseCameraControl();
			glViewport(0, 0, WIDTH, HEIGHT);
			ClearColourAndDepth(viewportColour);
			
			texShader.Select();
			glEnableTransparency(true);
			glEnableDepth(true);
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			float bscale = 2.f;
			Renderer::DrawTextureFullScreen(texShader, background, COLOUR_WHITE, bscale);
			glEnableDepth(false);
			Renderer::DrawTexture(texShader, texture, COLOUR_GREEN, camera.projectionViewMatrix, mat4(1.f), 0.01f);
			
		}

		void CreateTexture(int width, int height)
		{
			unsigned int id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			ImageData idata;
			idata.Load(ASSET_TEXTURE_PATH + "heart.jpg");

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, idata.width, idata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//glBindTexture(GL_TEXTURE_2D, texture);
			//vec4 emptyData = vec4(0, 0, 1, 1);

			

		//	std::vector<unsigned char> emptyData(width * height * 4, 150);
		//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, idata.width, idata.height, GL_RGBA, GL_UNSIGNED_BYTE, &emptyData[0]);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, idata.width, idata.height, GL_RGBA, GL_UNSIGNED_BYTE, &idata.data[0]);
			glBindTexture(GL_TEXTURE_2D, 0);
			checkGLError("create texture 2d array check");

		//	textureID = id;
			texture.id = id;
		}

		void ClearTexture(const vec4& col)
		{
			//glClearTexSubImage(textureID, 0, 0, 0, 0, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, &col);
		}
	};
}

