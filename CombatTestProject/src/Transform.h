#pragma once
#include "NativeHeader.h"
#include "glm\gtx\euler_angles.hpp"

namespace Native
{
	class Transform
	{
	public:

		Transform()
		{
			LocalPosition = glm::vec3(0.f, 0.f, 0.f);
			LocalRotation = glm::quat();
			LocalScale = glm::vec3(1.f, 1.f, 1.f);
			Parent = NULL;

			setPosition(glm::vec3(0.f, 0.f, 0.f));
			setRotation(glm::quat());
			setScale(glm::vec3(1.f, 1.f, 1.f));
			GetWorldMatrix();
		};

		~Transform() {
			
			Parent = NULL;
			for (Transform* t : Children)
				t->Parent = NULL;
		
		};

		glm::vec3 WorldPosition;
		glm::quat WorldRotation;
		glm::vec3 WorldScale;
		glm::vec3 LocalPosition;
		glm::quat LocalRotation;
		glm::vec3 LocalScale;
		glm::mat4 worldMatrix;
		glm::mat4 rotationMatrix;
		Transform* Parent;
		std::vector<Transform*> Children;

		void setPosition(const glm::vec3& value);
		glm::vec3 getPosition();
		glm::quat SetDirection(const glm::vec3& axis, const glm::vec3& direction);
		void setEulerXYZ(const glm::vec3& eulerAngles);
		void setEulerXYZ(float Xrot, float Yrot, float Zrot);
		void setEuler(const glm::vec3& eulerAngles);
		void setEuler(float Xrot,float Yrot,float Zrot);
		void setRotation(const glm::quat& value);
		glm::quat getRotation();
		void setScale(const glm::vec3& value);
		glm::vec3 getScale();
		void SetLocalPosition(const glm::vec3& value);
		glm::vec3 getLocalPosition();
		void SetLocalRotation(const glm::quat& value);
		glm::quat getLocalRotation();
		void SetLocalScale(const glm::vec3& value);
		glm::vec3 getLocalScale();
		glm::mat4 GetWorldMatrix();
		glm::mat4 GetTranslationMatrix();
		glm::mat4 GetScaleMatrix();
		glm::mat4 GetRotationMatrix();

		glm::vec3 getDirection(float x, float y, float z)
		{
			return glm::vec3(rotationMatrix*glm::vec4(x, y, z,0.f));
			//return WorldRotation * vec3(x, y, z);
		}

		glm::vec3 getDirection(const glm::vec3& vec)
		{
			return getRotation()*vec;
		}

		glm::vec3 getForward()
		{
			return getDirection(0, 0, 1);
		}

		glm::vec3 getUp()
		{
			return getDirection(0.f, 1.f, 0.f);
		}

		glm::vec3 getRight()
		{
			return getDirection(1.f, 0.f, 0.f);
		}

		glm::vec3 getForwardPosition()
		{
			return getPosition() + getDirection(0.f, 0.f, 1.f);
		}

		glm::vec3 getUpPosition()
		{
			return getPosition() + getDirection(0.f, 1.f, 0.f);
		}

		glm::vec3 getRightPosition()
		{
			return getPosition() + getDirection(1.f, 0.f, 0.f);
		}

		glm::vec3 TransformPoint(const vec3& point)
		{
			return vec3(worldMatrix*vec4(point, 1.f));
		}

		static void coutglm(const char * text,const glm::vec3& vec)
		{
			cout << text << "\n";
			cout << "	X: " << vec.x << "  Y: " << vec.y << "  Z: " << vec.z<<"\n";
		}

		static void coutglm(const char * text, const glm::quat& vec)
		{
			cout << text << "\n";
			cout << "	X: " << vec.x << "  Y: " << vec.y << "  Z: " << vec.z <<"  W: "<<vec.w<<"\n";
		}

		void Debug(const char* text)
		{
			cout <<"\n"<< text << "\n";
			coutglm("WorldPosition: ", getPosition());
			coutglm("WorldRotation: ", getRotation());
			coutglm("WorldScale: ", getScale());

			coutglm("LocalPosition: ", getLocalPosition());
			coutglm("LocalRotation: ", getLocalRotation());
			coutglm("LocalScale: ", getLocalScale());
			if(Parent)
			{
				cout << "\nParent\n";
				coutglm("Parent WorldPosition: ", Parent->getPosition());
				coutglm("Parent WorldRotation: ", Parent->getRotation());
				coutglm("Parent WorldScale: ", Parent->getScale());

				coutglm("Parent LocalPosition: ", Parent->getLocalPosition());
				coutglm("Parent LocalRotation: ", Parent->getLocalRotation());
				coutglm("Parent LocalScale: ", Parent->getLocalScale());
			}
			else{
				cout << "\nNo Parent\n";
			}
		}

		static mat4 Scale(float X, float Y, float Z)
		{
			return glm::scale(mat4(1.f), vec3(X, Y, Z));
		}

		static mat4 Translate(float X, float Y, float Z)
		{
			return glm::translate(mat4(1.f), vec3(X, Y, Z));
		}

		static mat4 GetEulerXYZ(float Xrot, float Yrot, float Zrot)
		{
			return GetEuler(Xrot, Zrot, Yrot);
		}

		static mat4 GetEuler(float Xrot, float Yrot, float Zrot)
		{
			vec3 euler = vec3(glm::radians(Xrot), glm::radians(Yrot), glm::radians(Zrot));
			mat3 rotm = glm::orientate3(euler);
			quat q = glm::toQuat(rotm);
			return glm::toMat4(q);
		}
	};
}

