#pragma once
#include "NativeHeader.h"
#include "GPUMesh.h"
#include "Shader.h"
#include "Modal.h"
#include "ShaderFactory.h"

namespace Native {
	class SkinnedMeshShader : public Shader
	{
	public:

		void BuildAndCompile()
		{
			checkGLError("Checking pre animation shader compile...");
			Shader s = ShaderFactory::GetSkinnedMeshShaderDefered();
			vertexString = s.vertexString;
			geometryString = s.geometryString;
			fragmentString = s.fragmentString;
			Compile("Skinned Mesh Shader");
			checkGLError("Checking post animation shader compile...");
			initialized = true;
		}

		void BuildAndCompileInstanced()
		{
			checkGLError("Checking pre animation shader compile...");
			Shader s = ShaderFactory::GetSkinnedMeshShaderDeferedInstanced();
			vertexString = s.vertexString;
			geometryString = s.geometryString;
			fragmentString = s.fragmentString;
			Compile("Skinned Mesh Instanced Shader");
			checkGLError("Checking post animation shader compile...");
			initialized = true;
		}
	};
}
