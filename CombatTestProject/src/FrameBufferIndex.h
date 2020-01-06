#pragma once
#include "NativeHeader.h"
#include "Input.h"
#include "Framebuffer.h"

nativenamespace{

	class FramebufferIndex : public Framebuffer
	{
	private:
		//int _numDrawBuffers;
		//unsigned int _depthTexture;

	public:

		FramebufferIndex()
		{
			clearColour = vec4(0.1f,0.f,0.1f,1.f);
		}

		unsigned int colourTextureID;
		unsigned int positionTextureID;
		unsigned int indexTextureID;
		int fbWidth;
		int fbHeight;
		unsigned int colourAttachment = 0;
		unsigned int positionAttachment = 1;
		unsigned int indexAttachment = 2;
		vec3 testLight;
		vec4 clearColour;

		bool CreateIndexFramebuffer(unsigned int Width, unsigned int Height)
		{
			//generate frame buffer
			GenFramebuffer();
			Bind();
			fbWidth = Width;
			fbHeight = Height;
			// create depth texture
			depthTexture = CreateDepthTexture24(Width, Height);
			//attach depth texture
			SetDepthAttachment(depthTexture);

			//attach color texture layers
			colourTextureID = CreateTexture32F(Width, Height);
			SetColourAttachment(colourTextureID, colourAttachment);

			positionTextureID = CreateTexture32F(Width, Height);
			SetColourAttachment(positionTextureID, positionAttachment);

			indexTextureID = CreateTextureUInt(Width, Height);
			SetColourAttachment(indexTextureID, indexAttachment);
			//set draw buffers
			attachmentCount = 3;
			SetDrawBuffer(attachmentCount);

			bool status = CheckStatus();
			Unbind();
			return status;
		}

		void RebuildFramebuffer(int width,int height) override
		{
			CreateIndexFramebuffer(width, height);
		}

		void BlitFromDefault()
		{
			SetDrawBuffer(1);
			Framebuffer::BlitFromDefault(fbWidth, fbHeight);
			SetDrawBuffer();
		}

		void BlitToDefault()
		{
			//SetDrawBuffer(1);
			Framebuffer::BlitToDefault(fbWidth, fbHeight);
		}

		void Clear()
		{
			ClearAttachmentF(vec4(0),colourAttachment);
			ClearAttachmentF(vec4(0),positionAttachment);
			ClearAttachmentUint(uvec4(0),indexAttachment);
			ClearDepthAttachment(1.f);
		}

		uvec4 ReadIDBuffer()
		{
			return ReadIDBuffer(Input::mouse.x, Input::mouse.y);
		}

		vec4 RaedPositionBuffer()
		{
			return ReadPositionBuffer(Input::mouse.x, Input::mouse.y);
		}

		uvec4 ReadIDBuffer( double x, double y)
		{
			return ReadAttachmentUvec4(x, y,indexAttachment,  fbWidth, fbHeight);
		}

		vec4 ReadPositionBuffer(double x, double y)
		{
			return ReadAttachmentVec4(x, y,positionAttachment, fbWidth, fbHeight);
		}
	};
}

