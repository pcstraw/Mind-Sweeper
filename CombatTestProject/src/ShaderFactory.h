#pragma once
#include "Shader.h"

nativenamespace{

	class ShaderFactory
	{
	public:

		static Shader GetMeshShader()
		{
			Shader shader;
			shader.vertexString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "mesh.vert");
			shader.geometryString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "mesh.geom");
			shader.fragmentString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "mesh.frag");
			return shader;
		}

		static Shader GetMeshShaderDefered()
		{
			Shader shader;
			shader.vertexString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "mesh.vert");
			shader.geometryString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "mesh.geom");
			shader.fragmentString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "meshDefered.frag");
			return shader;
		}

		static Shader GetSkinnedMeshShaderDefered()
		{
			Shader shader;
			shader.vertexString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "skinnedMesh.vert");
			shader.geometryString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "skinnedMesh.geom");
			shader.fragmentString = Shader::LoadFile( SHADER_GEOMETRY_PATH + "skinnedMeshDefered.frag");
			return shader;
		}

		static Shader GetSkinnedMeshShaderDeferedInstanced()
		{
			Shader shader;
			shader.vertexString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "skinnedMeshInstanced.vert");
			shader.geometryString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "skinnedMesh.geom");
			shader.fragmentString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "skinnedMeshDefered.frag");
			return shader;
		}

		static Shader GetDeferedLighting()
		{
			Shader shader;
			shader.vertexString = Shader::LoadFile(SHADER_LIGHTING_PATH + "deferedLighting.vert");
			shader.fragmentString = Shader::LoadFile(SHADER_LIGHTING_PATH + "deferedLighting.frag");
			return shader;
		}

		static Shader GetGimbalRotShader()
		{
			Shader shader;
			shader.vertexString   = Shader::LoadFile(SHADER_GEOMETRY_PATH + "gimbalRot.vert");
			shader.geometryString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "gimbalRot.geom");
			shader.fragmentString = Shader::LoadFile(SHADER_GEOMETRY_PATH + "gimbalRot.frag");
			return shader;
		}

		static Shader GetSDFDebugger()
		{
			Shader shader;
			shader.vertexString = Shader::LoadFile("shaders\\deferedVoxelVertex.vert");
			shader.fragmentString = Shader::LoadFile("shaders\\debugSDF.frag");
			shader.Compile("Debug SDF");
			return shader;
		}

		static Shader GetTesselationShader()
		{
			Shader shader;
			shader.vertexString = Shader::LoadFile("shaders\\Tessellation\\tess.vert");
			//tessShader.geometryString = Shader::LoadFile("shaders\\Tessellation\\tess.geom");
			shader.fragmentString = Shader::LoadFile("shaders\\Tessellation\\tess.frag");
			shader.tessellationControlString = Shader::LoadFile("shaders\\Tessellation\\control.tess");
			shader.tessellationEvaluationString = Shader::LoadFile("shaders\\Tessellation\\evaluation.tess");
			return shader;
		}
	};
}
