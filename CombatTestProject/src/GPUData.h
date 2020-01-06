#pragma once
#include "NativeHeader.h"
#include "Utils.h"

nativenamespace{

	enum class BufferDataType {

	None = 0,Float,Float2,Float3,Float4,Mat3,Mat4,Int,Int2,Int3,Int4 ,Uint,Uint2,Uint3,Uint4,Bool
};

static uint32_t BufferDataTypeStride(const BufferDataType& type)
{
	switch (type)
	{
	case BufferDataType::Float:     return 4;
	case BufferDataType::Float2:     return 4 * 2;
	case BufferDataType::Float3:     return 4 * 3;
	case BufferDataType::Float4:     return 4 * 4;
	case BufferDataType::Mat3:     return 4 * 3 * 3;
	case BufferDataType::Mat4:     return 4 * 4 * 4;
	case BufferDataType::Int:     return 4;
	case BufferDataType::Int2:     return 4 * 2;
	case BufferDataType::Int3:     return 4 * 3;
	case BufferDataType::Int4:     return 4 * 4;
	case BufferDataType::Uint:     return 4;
	case BufferDataType::Uint2:     return 4 * 2;
	case BufferDataType::Uint3:     return 4 * 3;
	case BufferDataType::Uint4:     return 4 * 4;
	case BufferDataType::Bool:     return 1;
	}
	return 0;
}


static GLenum BufferDataTypeToOpenGLBaseType(BufferDataType type)
{
	switch (type)
	{
	case BufferDataType::Float:     return GL_FLOAT;
	case BufferDataType::Float2:     return GL_FLOAT;
	case BufferDataType::Float3:     return GL_FLOAT;
	case BufferDataType::Float4:     return GL_FLOAT;
	case BufferDataType::Mat3:     return GL_FLOAT;
	case BufferDataType::Mat4:     return GL_FLOAT;
	case BufferDataType::Int:     return GL_INT;
	case BufferDataType::Int2:    return GL_INT;
	case BufferDataType::Int3:     return GL_INT;
	case BufferDataType::Int4:     return GL_INT;
	case BufferDataType::Uint:     return GL_UNSIGNED_INT;
	case BufferDataType::Uint2:     return GL_UNSIGNED_INT;
	case BufferDataType::Uint3:    return GL_UNSIGNED_INT;
	case BufferDataType::Uint4:     return GL_UNSIGNED_INT;
	case BufferDataType::Bool:     return GL_INT;
	}
	return 0;
}

static int BufferDataTypeBaseSize(BufferDataType type)
{
	switch (type)
	{
	case BufferDataType::Float:     return sizeof(float);
	case BufferDataType::Float2:     return sizeof(float);
	case BufferDataType::Float3:     return sizeof(float);
	case BufferDataType::Float4:     return sizeof(float);
	case BufferDataType::Mat3:     return sizeof(float);
	case BufferDataType::Mat4:     return sizeof(float);
	case BufferDataType::Int:     return sizeof(int);
	case BufferDataType::Int2:    return sizeof(int);
	case BufferDataType::Int3:     return sizeof(int);
	case BufferDataType::Int4:     return sizeof(int);
	case BufferDataType::Uint:     return sizeof(unsigned int);
	case BufferDataType::Uint2:     return sizeof(unsigned int);
	case BufferDataType::Uint3:    return sizeof(unsigned int);
	case BufferDataType::Uint4:     return sizeof(unsigned int);
	case BufferDataType::Bool:     return sizeof(int);
	}
	return 0;
}

class ElementBuffer {
public:
	ElementBuffer() {}
	string name;
	unsigned int id;
	int length;

	int LoadIndices(const std::vector<uvec3>& indices)
	{
		return LoadIndices(Utils::GetUIntArray(indices));
	}

	int LoadIndices(const std::vector<unsigned int>& indices)
	{
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

		int indexsize = indices.size();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexsize, &indices[0], GL_STATIC_DRAW);
		return length = indexsize;
	}
};

class ArrayBuffer {
public:
	ArrayBuffer() {}
	string name;
	unsigned int id;
	int length;
	int layoutIndex;
	int stride;
	int typeSize;
	BufferDataType dataType;
	GLenum glType;

	void EnableAttributePointer(int LayoutIndex)
	{
		layoutIndex = LayoutIndex;
		glEnableVertexAttribArray(LayoutIndex);
		glBindVertexBuffer(LayoutIndex, id, 0, stride);
	}

	ArrayBuffer(const std::vector<float>& data) { LoadData(data); }
	int LoadData( const std::vector<float>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<float> _dat = data;
		SetDataType(BufferDataType::Float);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<vec2>& data) { LoadData(data); }
	int LoadData(const std::vector<vec2>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<float> _dat = Utils::GetFloatArray(data);
		SetDataType(BufferDataType::Float2);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<vec3>& data) { LoadData(data); }
	int LoadData(const std::vector<vec3>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<float> _dat = Utils::GetFloatArray(data);
		SetDataType(BufferDataType::Float3);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<vec4>& data) { LoadData(data); }
	int LoadData(const std::vector<vec4>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<float> _dat = Utils::GetFloatArray(data);
		SetDataType(BufferDataType::Float4);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<int>& data) { LoadData(data); }
	int LoadData(const std::vector<int>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<int> _dat = data;
		SetDataType(BufferDataType::Int);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<ivec2>& data) { LoadData(data); }
	int LoadData(const std::vector<ivec2>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<int> _dat = Utils::GetIntArray(data);
		SetDataType(BufferDataType::Int2);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<ivec3>& data) { LoadData(data); }
	int LoadData( const std::vector<ivec3>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<int> _dat = Utils::GetIntArray(data);
		SetDataType(BufferDataType::Int3);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<ivec4>& data) { LoadData(data); }
	int LoadData(const std::vector<ivec4>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<int> _dat = Utils::GetIntArray(data);
		SetDataType(BufferDataType::Int4);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<unsigned int>& data) { LoadData(data); }
	int LoadData(const std::vector<unsigned int>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<unsigned int> _dat = data;
		SetDataType(BufferDataType::Uint);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<uvec2>& data) { LoadData(data); }
	int LoadData( const std::vector<uvec2>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<unsigned int> _dat = Utils::GetUIntArray(data);
		SetDataType(BufferDataType::Uint2);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<uvec3>& data) { LoadData(data); }
	int LoadData(const std::vector<uvec3>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<unsigned int> _dat = Utils::GetUIntArray(data);
		SetDataType(BufferDataType::Uint3);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	ArrayBuffer(const std::vector<uvec4>& data) { LoadData(data); }
	int LoadData(const std::vector<uvec4>& data, const string& Name = "ArrayBuffer")
	{
		std::vector<unsigned int> _dat = Utils::GetUIntArray(data);
		SetDataType(BufferDataType::Uint4);
		name = Name;
		int size = _dat.size() + 1;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, typeSize*size, &_dat[0], GL_STATIC_DRAW);
		return length = size;
	}

	void SetDataType(BufferDataType _bufferDataType)
	{
		dataType = _bufferDataType;
		glType = BufferDataTypeToOpenGLBaseType(dataType);
		stride = BufferDataTypeStride(dataType);
		typeSize = BufferDataTypeBaseSize(dataType);
	}

private: 

	
};


class ShaderStorageBuffer {
public:

	unsigned int index;
	unsigned int id;
	int length;

	int AllocateStorage(int Index, int size)
	{
		index = Index;
		glGenBuffers(1, &id);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Index, id);

		int _size = size;
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, _size, NULL, GL_DYNAMIC_STORAGE_BIT);
		checkGLError("Storage buffer allocation check");
		return length = size;
	}

	int LoadData(unsigned int Index, const std::vector<float>& data)
	{
		index = Index;
		glGenBuffers(1, &id);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Index, id);

		int size = data.size();
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*size, &data[0], GL_STATIC_DRAW);
		return length = size;
	}

	int LoadData(unsigned int Index, const std::vector<int>& data)
	{
		index = Index;
		glGenBuffers(1, &id);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Index, id);

		int size = data.size();
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*size, &data[0], GL_STATIC_DRAW);
		return length = size;
	}

	int LoadData(unsigned int Index, const std::vector<unsigned int>& data)
	{
		index = Index;
		glGenBuffers(1, &id);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Index, id);

		int size = data.size();
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int)*size, &data[0], GL_STATIC_DRAW);
		return length = size;
	}
};
}
