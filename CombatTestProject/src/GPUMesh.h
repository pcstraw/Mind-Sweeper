#pragma once
#include "NativeHeader.h"
#include "GPUData.h"
#include "Vertex.h"
#include "Faces.h"
//#include "Vertex.h"
//#include "BufferLayout.h"
//#include "Faces.h"

namespace Native
{
	class GPUMesh
	{
	public:
		unsigned int ID = 0;
		ElementBuffer indexBuffer;
		std::vector<ArrayBuffer> arrayBuffers;

		void BufferMesh(std::vector<Face>& faces,bool hasBones = false)
		{
			std::vector<unsigned int> indices;
			std::vector<Vertex> vertices;
			int index = 0;
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				for (unsigned int j = 0; j < 3; j++)
				{
					vertices.push_back(faces[i].points[j]);
					indices.push_back(index);
					index++;
				}
			}

			indexBuffer.LoadIndices(indices);
			BufferMesh(vertices,hasBones);
		}

		void BufferMesh(std::vector<vec3>& vertices)
		{
			std::vector<unsigned int> indices;
			for (unsigned int i = 0; i < vertices.size()*3; i++)
				indices.push_back(i);

			indexBuffer.LoadIndices(indices);
			BufferVertices(vertices);
		}

		void BufferMesh(const std::vector<vec3>& vertices, const std::vector<uvec3>& indices )
		{
			indexBuffer.LoadIndices(indices);
			BufferVertices(vertices);
		}

		void BufferMesh(const std::vector<Vertex>& vertices, bool hasBones = false)
		{
			std::vector<unsigned int> indices;
			for (unsigned int i = 0; i < vertices.size() * 3; i++)
				indices.push_back(i);

			BufferMesh(vertices, indices,hasBones);
		}

		void BufferMesh(const std::vector<Vertex>& vertices, const std::vector<uvec3>& indices, bool hasBones = false)
		{
			BufferMesh(vertices, Utils::GetUIntArray(indices), hasBones);
		}

		void BufferMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,bool hasBones = false)
		{
			indexBuffer.LoadIndices(indices);
			arrayBuffers = Vertex::GetArrayBuffer(vertices,hasBones);
			
			GenVAO();
			BindAttribPointers();

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			checkGLError("GPUMesh: Pre gpu buffer check");
		}

		void BufferVertices(const std::vector<vec3>& vertices)
		{
			ArrayBuffer positionBuffer;
			positionBuffer.LoadData(vertices);
			arrayBuffers.push_back(positionBuffer);

			GenVAO();
			BindAttribPointers();

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			checkGLError("GPUMesh: Pre gpu buffer check");
		}

		void BindAttribPointers()
		{
			BindVAO();
			for (int i = 0; i < arrayBuffers.size(); i++)
				arrayBuffers[i].EnableAttributePointer(i);
			ResetVAO();
		}

		void DrawMesh(int instanceCount)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer.id);
			glDrawElementsInstanced(GL_TRIANGLES, indexBuffer.length, GL_UNSIGNED_INT, 0, instanceCount);
		}

		void DrawMesh()
		{
			BindVAO();
			checkGLError("Bind VAO check");
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer.id);
			checkGLError("Bind Element buffer check");
			glDrawElements(GL_TRIANGLES, indexBuffer.length, GL_UNSIGNED_INT, 0);
			checkGLError("Draw Elements check");
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			checkGLError("post VAO draw check");
		}

		void debugMe(const char* text)
		{
			cout << "\nVAO: debugMe: " << text << endl;
			cout << "-> ID: " << ID << endl;
			cout << endl;
		}

		void GenVAO()
		{
			glGenVertexArrays(1, &ID);
		}

		void BindVAO()
		{
			glBindVertexArray(ID);
		}

		void ResetVAO()
		{
			glBindVertexArray(0);
		}

	private:
	};
}