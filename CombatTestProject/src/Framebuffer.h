#pragma once
#include "NativeHeader.h"
#include "Viewport.h"

nativenamespace{

	class Framebuffer
	{
		
	public:

		unsigned int textureArrayID;
		unsigned int frameBufferID = 0;
		unsigned int depthTexture;
		unsigned int attachmentCount =1;
		int arrayLayers = 4;
		vec4 clearColour;

		void Begin()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
			glClearColor(clearColour.x,clearColour.y,clearColour.z,clearColour.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}

		void End()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void BlitToDefault(int width, int height, GLenum mask, GLenum Filter)
		{
			glBlitNamedFramebuffer(frameBufferID,0,0,0,width,height,0,0,width,height,mask,Filter);
		}

		void BlitToDefault(int Width, int Height)
		{
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
			//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			//glReadBuffer(GL_COLOR_ATTACHMENT0);
			//glDrawBuffer(GL_COLOR_ATTACHMENT0);
			//BlitToDefault(Width, Height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			glBlitNamedFramebuffer(frameBufferID, 0, 0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}

		void BlitFromDefault(int Width, int Height)
		{
			checkGLError("Blit from buffer 0 pre check");
			//SetDrawBuffer(1);
			glBlitNamedFramebuffer(0, frameBufferID, 0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			checkGLError("Blit from buffer 0 post check");
			//SetDrawBuffer(3);
		}

		void BlitDpethToDefaultBuffer(int Width, int Height)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
			glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height,
				GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		}

		void ClearAttachmentF(const vec4& value,unsigned int attachment)
		{
			glClearNamedFramebufferfv(frameBufferID, GL_COLOR, attachment, &value[0]);
		}
		void ClearAttachmentUint(const uvec4& value,unsigned int attachment)
		{
			glClearNamedFramebufferuiv(frameBufferID, GL_COLOR, attachment, &value[0]);
		}
		void ClearDepthAttachment(float value)
		{
			glClearNamedFramebufferfv(frameBufferID, GL_DEPTH, 0, &value);
		}

		void ClearTextureArray(const vec4& col)
		{
			glClearTexSubImage(textureArrayID, 0, 0, 0, 0, WIDTH, HEIGHT, 4, GL_RGBA, GL_FLOAT, &col);
		}

		unsigned int CreateTexture32F(int Width, int Height)
		{
			unsigned int id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Width, Height, 0, GL_RGBA, GL_FLOAT,
				NULL);
			//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_2D, 0);
			checkGLError("create texture 2d array check");
			//glTextureStorage3D(id, 1, GL_RGBA16F, width, height, size);
			//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);

			return id;
		}

		unsigned int CreateTextureUInt(int Width, int Height)
		{
			unsigned int id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, Width, Height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
			//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_2D, 0);
			checkGLError("create texture 2d array check");
			//glTextureStorage3D(id, 1, GL_RGBA16F, width, height, size);
			//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);

			return id;
		}

		void CreateTextureArray32F(int width, int height, int size)
		{
			checkGLError("create texture 2d array check");
			unsigned int id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D_ARRAY, id);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);
			checkGLError("create texture 2d array check");
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
			checkGLError("create texture 2d array check");

			textureArrayID = id;
		}

		unsigned int CreateTexture32UInt(int Width, int Height)
		{
			unsigned int id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, Width, Height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
			//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_2D, 0);
			checkGLError("create texture 2d array check");
			//glTextureStorage3D(id, 1, GL_RGBA16F, width, height, size);
			//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);

			return id;
		}

		unsigned int CreateTextureArray32F(int Width, int Height)
		{
			unsigned int id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Width, Height, 0, GL_RGBA, GL_FLOAT,
				NULL);
			//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_2D, 0);
			checkGLError("create texture 2d array check");
			//glTextureStorage3D(id, 1, GL_RGBA16F, width, height, size);
			//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, width, height, size, 0, GL_RGBA, GL_FLOAT, NULL);

			return id;
		}

		unsigned int CreateDepthTexture24(unsigned int WindowWidth, unsigned int WindowHeight)
		{
			unsigned int id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
				NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);

			return id;
		}

		bool CreateDeferedFramebuffer(int WindowWidth, int WindowHeight, int layerCount = 4)
		{
			arrayLayers = layerCount;
			return BuildDeferedFramebuffer(WindowWidth, WindowHeight, arrayLayers);
		}

		bool BuildDeferedFramebuffer(int WindowWidth, int WindowHeight, int layerCount = 4)
		{
			GenFramebuffer();
			Bind();

			// create depth texture
			checkGLError("Frame buffer textures pre check 1");
			depthTexture = CreateDepthTexture24(WindowWidth, WindowHeight);
			checkGLError("Frame buffer textures pre check 2");
			//attach depth texture
			SetDepthAttachment(depthTexture);

			checkGLError("Frame buffer textures pre check 3");
			//create color texture array
			CreateTextureArray32F(WindowWidth, WindowHeight, layerCount);
			//glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
			//TODO: extra to seperate function
			//attach color texture layers
			for (unsigned int i = 0; i < layerCount; i++) {
				SetColourAttachmentLayered(textureArrayID, i, i);
				//glNamedFramebufferTextureLayer(frameBufferID, GL_COLOR_ATTACHMENT0 + i, textureArrayID, 0, i);
				//glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textureArrayID, 0, i);
			}

			//set draw buffers
			attachmentCount = layerCount;
			SetDrawBuffer();
			bool status = CheckStatus();
			Unbind();
			//glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
			//glBindTexture(GL_TEXTURE_2D, 0);
			return status;
		}

		void virtual RebuildFramebuffer(int Width, int Height)
		{
			BuildDeferedFramebuffer(Width, Height,arrayLayers);
		}

		void SetDrawBuffer()
		{
			SetDrawBuffer(attachmentCount);
		}

		void SetDrawBuffer(unsigned int numAttachments)
		{
			std::vector<unsigned int> attachments;
			attachments.resize(numAttachments);
			for (int i = 0; i < numAttachments; i++)
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			unsigned int* _att = &attachments[0];
			glDrawBuffers(numAttachments, _att);
		}

		bool CheckStatus()
		{
			//check buffer status
			GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (Status != GL_FRAMEBUFFER_COMPLETE) {
				cout <<">>>>> ERROR <<<<<< FrameBuffer incomplete! status code: "<< Status << endl;
				return false;
			}
			return true;
		}

		void SetColourAttachment(unsigned int texture,unsigned int attachment)
		{
			glNamedFramebufferTexture(frameBufferID, GL_COLOR_ATTACHMENT0 + attachment, texture, 0);
			checkGLError("Set Colour attachment check");
		}

		void SetDepthAttachment(unsigned int texture)
		{
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
			glNamedFramebufferTexture(frameBufferID, GL_DEPTH_ATTACHMENT, texture, 0);
		}

		void SetColourAttachmentLayered(unsigned int texture,unsigned int attachment,unsigned int layer)
		{
			glNamedFramebufferTextureLayer(frameBufferID, GL_COLOR_ATTACHMENT0 + attachment, texture, 0, layer);
		}

		void BindWindowViewport()
		{
			BindViewport(Viewport::window);
		}

		void BindViewport(Viewport* viewport)
		{
			std::function<void(int, int)> _f = std::bind(&Framebuffer::onResize, this, std::placeholders::_1, std::placeholders::_2);
			Viewport::window->callbacks.push_back(_f);
		}

		void onResize(int width, int height)
		{
			DeleteTextures();
			RebuildFramebuffer(width, height);
		}

		void virtual DeleteTextures()
		{
			glDeleteTextures(1, &depthTexture);
			glDeleteTextures(1, &textureArrayID);
		}

		unsigned int GenFramebuffer()
		{
			if (frameBufferID == 0)
			{
				glGenFramebuffers(1, &frameBufferID);
				BindWindowViewport();
			}
			return frameBufferID;
		}

		void Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		}

		static void Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		uvec4 ReadAttachmentUvec4(double x, double y,unsigned int attachment,int Width, int Height)
		{
			checkGLError("Pre Read pixels bind check");
			glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
			glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			checkGLError("Read pixels bind check");

			uvec4 indexvec = uvec4(0);
			glReadPixels((GLint)x, (GLint)(Height - y), (GLsizei)1, (GLsizei)1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &indexvec);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			checkGLError("post read pixels read");
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			return indexvec;
		}

		vec4 ReadAttachmentVec4(double x, double y,unsigned int attachment,int width, int height)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
			glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
			vec4 pos;
			glReadPixels((GLuint)x, (GLuint)(height - y), (GLsizei)1, (GLsizei)1, GL_RGB, GL_FLOAT, &pos);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			return pos;
		}
	};
}