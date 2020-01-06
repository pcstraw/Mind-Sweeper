#pragma once
#include "NativeHeader.h"
#include "SOIL\SOIL.h"
#include "SOIL\image_helper.h"
#include "GLResource.h"

nativenamespace{

	class ImageData
	{
	public:
		
		string filePath;
		int width;
		int height;
		unsigned char* data;

		bool Load(const string& path)
		{
			return Get(path, *this);
		}

		static bool Get(const string& FilePath, ImageData& InData)
		{
			if (FilePath.length() == 0)
			{
				cout << "\n >>>> ERROR! Texture2d: LoadFromFile: error: path is null" << endl;
				return false;
			}

			InData.data = SOIL_load_image(FilePath.c_str(), &InData.width, &InData.height, 0, SOIL_LOAD_RGBA);

			if (InData.data == NULL)
			{
				cout << "\n >>>> ERROR! Texture2d: failed to load image. Check the file path: " << FilePath << endl;
				return false;
			}
			InData.filePath = FilePath;
			return true;
		}

		~ImageData()
		{
			SOIL_free_image_data(data);
		}
	};

	class TextureResource : public GLResource
	{
	public:

		TextureResource() : GLResource()
		{}

		void Delete() override
		{
			//if (_deleted)
			//	return;
			glDeleteTextures(1, &id);
			//_deleted = true;
		}
	};

	class Texture2d {

	public:

		Texture2d()
		{
			id = 0;
			width = 0;
			height = 0;
		}

		unsigned int id;
		string filePath;
		int width;
		int height;

		static Texture2d LoadFromFile(const string& path)
		{
			Texture2d tex;
			tex.Load(path);
			return tex;
		}

		unsigned int Load(const string& path)
		{
			ImageData imageData;
			if (!ImageData::Get(path, imageData))
				return 0;

			filePath = path;
			width = imageData.width;
			height = imageData.height;

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageData.width, imageData.height, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, imageData.data);

			glBindTexture(GL_TEXTURE_2D, 0);

			TextureResource* tr = new TextureResource();
			tr->id = id;
			GLResource::AddResource(tr);

			cout << "::Texture2d: Loaded Texture: " << path << endl;
			return id;

			/*
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			ImageData idata;
			idata.Load(path);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, idata.width, idata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &idata.data[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, idata.width, idata.height, GL_RGBA, GL_UNSIGNED_BYTE, &idata.data[0]);
			glBindTexture(GL_TEXTURE_2D, 0);
			checkGLError("create texture 2d array check");

			width = idata.width;
			height = idata.height;
			filePath = path;
			*/
				//	textureID = id;
			//texture.id = id;
		}

		void Bind(int textureUint)
		{
			if (id == 0)
				cout << "\n >>>> WARNING! Texture2d: texture id is 0" << endl;

			//glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, id);
		}

		void Release()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		ImageData GetImageData()
		{
			ImageData idata;
			ImageData::Get(filePath, idata);
			return idata;
		}
	};
}
