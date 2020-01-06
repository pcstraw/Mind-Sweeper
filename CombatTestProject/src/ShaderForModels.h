#pragma once
#include "NativeHeader.h"
#include "Shader.h"
#include "ShaderFactory.h"
#include "Renderer.h"

nativenamespace{

	class ShaderForModels : public Shader
	{
	public:

		void Build(const string& ShaderName)
		{
			Shader s = ShaderFactory::GetMeshShaderDefered();
			vertexString = s.vertexString;
			geometryString = s.geometryString;
			fragmentString = s.fragmentString;
			Compile(ShaderName);
		}
	};

}
