#pragma once
#include "Shader.h"
#include "Modal.h"
#include "Camera.h"
#include "Texture2d.h"

nativenamespace{

class Renderer {

public:

	static void DrawMesh(Shader& shader, Camera* camera, Mesh& mesh)
	{
		mat4 model = mesh.transform.worldMatrix;
		mat4 viewmodel = camera->viewMatrix * model;
		mat4 projection = camera->projectionViewMatrix;
		shader.SetUniform4x4("model", model);
		shader.SetUniform4x4("viewmodel", viewmodel);
		shader.SetUniform4x4("projection", projection);
		shader.SetUniform4f("colour", mesh.colour);
		//glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		
		mesh.gpuData.DrawMesh();
	}

	static void DrawMesh(Shader& shader, const Camera& camera,const mat4& model, Mesh& mesh)
	{
		mat4 viewmodel = camera.viewMatrix * model;
		mat4 projection = camera.projectionViewMatrix;
		shader.SetUniform4x4("model", model);
		shader.SetUniform4x4("viewmodel", viewmodel);
		shader.SetUniform4x4("projection", projection);
		shader.SetUniform4f("colour", mesh.colour);
		mesh.gpuData.DrawMesh();
	}

	static void DrawMesh(Shader& shader,GPUMesh& mesh, const Camera& camera,const mat4& model, const vec4& colour)
	{
		mat4 viewmodel = camera.viewMatrix * model;
		mat4 projection = camera.projectionViewMatrix;
		shader.SetUniform4x4("model", model);
		shader.SetUniform4x4("viewmodel", viewmodel);
		shader.SetUniform4x4("projection", projection);
		shader.SetUniform4f("colour", colour);
		mesh.DrawMesh();
	}

	static void DrawModels(Shader& shader,Camera& camera, std::vector<Model*>& models)
	{
		shader.Select();
		for (Model* m : models)
			Renderer::DrawModel(shader,camera, *m);
	}

	static void DrawModel(Shader& shader,Camera& camera, Model& model)
	{
		mat4 modelMatrix = model.transform.GetWorldMatrix();
		for (Mesh& m : model.meshes)
		{
			mat4 mmodel = modelMatrix * m.transform.GetWorldMatrix();
			mat4 viewmodel = camera.viewMatrix * mmodel;
			shader.SetUniform4x4("model", mmodel);
			shader.SetUniform4x4("viewmodel", viewmodel);
			shader.SetUniform4x4("projection", camera.projectionViewMatrix);
			shader.SetUniform4f("colour", m.colour);
			m.gpuData.DrawMesh();
		}
	}

	static void DrawTexture(Shader& shader,const Texture2d& texture,const vec4& colour,const mat4& projectionView,const mat4& transform,float size)
	{
		shader.SetProjection(projectionView);
		shader.SetModel(transform);
		shader.SetUniform4f("colour", colour);
		shader.SetUniform2f("size", vec2(texture.width,texture.height)*size);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		Shader::DrawQuad();
	}

	static void DrawTextureFullScreen(Shader& shader,const Texture2d& texture,const vec4& colour,float size)
	{
		mat4 iden = mat4(1.f);
		shader.SetProjection(iden);
		mat4 cp = glm::translate(iden, vec3(-1.0, -1.0, 0)*size);
		shader.SetModel(cp);
		shader.SetUniform4f("colour", colour);
		shader.SetUniform2f("size", vec2(2.f,2.f)*size);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		Shader::DrawQuad();
	}

	static void DrawSkinnedModels(Shader& shader, Camera* cam,std::vector<Model*>& models)
	{
		shader.Select();
		for (Model* m : models)
			DrawSkeleton(shader, *m, cam);
	}

	static void DrawSkeleton(Shader& shader,Model& model, Camera* cam)
	{
		model.transform.GetWorldMatrix();
		shader.SetUniform4x4("projection", cam->projectionViewMatrix);
		shader.SetUniform4x4("model", model.transform.worldMatrix);
		shader.SetUniform4x4("viewmodel", cam->viewMatrix*model.transform.worldMatrix);
		for (Mesh& mesh : model.meshes)
		{
			//mesh.skeleton.Animate(0);
			glUniformMatrix4fv(shader.GetUniformID("gBones"), 100, GL_FALSE, glm::value_ptr(mesh.skeleton.boneMatrices.at(0)));
			shader.SetUniform4f("colour", mesh.colour);
			mesh.gpuData.DrawMesh();
		}
		checkGLError("post skeleton render check");
	}

	static void DrawSkeletonTimeStamp(Shader& shader,Mesh& mesh,const mat4& transform, const std::vector<mat4>& bonelist, const Camera& cam)
	{
		shader.SetUniform4x4("projection", cam.projectionViewMatrix);
		shader.SetUniform4x4("model", transform);
		shader.SetUniform4x4("view", cam.viewMatrix);
		glUniformMatrix4fv(shader.GetUniformID("gBones"), 100, GL_FALSE, glm::value_ptr(bonelist.at(0)));

		shader.SetUniform4f("colour", mesh.colour);
		mesh.gpuData.DrawMesh();
		checkGLError("post skeleton render check");
	}

	static void DrawSkeleton(Shader& shader,Mesh& mesh,Camera* cam)
	{
		mesh.transform.GetWorldMatrix();
		shader.SetUniform1i("colourid", mesh.selectionID);
		shader.SetUniform4x4("projection", cam->projectionViewMatrix*mesh.transform.worldMatrix);
		shader.SetUniform4x4("model", mesh.transform.worldMatrix);
		shader.SetUniform4f("colour",mesh.colour);
		shader.SetUniform4x4("viewmodel", cam->viewMatrix*mesh.transform.worldMatrix);
		glUniformMatrix4fv(shader.GetUniformID("gBones"), 100, GL_FALSE, glm::value_ptr(mesh.skeleton.boneMatrices.at(0)));

		mesh.gpuData.DrawMesh();
		checkGLError("post skeleton render check");
	}
};
}
