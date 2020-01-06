#pragma once
#include "NativeHeader.h"
#include "AssetDatabase.h"

nativenamespace{

	class GimbalRotation {

	public:

		int id;
		Model model;
		Transform transform;
		vec3 position;
		float scale = 1.f;
		mat4 moffset[3];
		vec4 coffset[3];
		vec3 hitPoint = vec3(0);
		vec3 clickPoint = vec3(0);
		vec3 oldhit = vec3(0);
		vec3 axisConstraint = vec3(0);
		vec3 localAxisConstraint = vec3(0);

		unsigned int hitID = 0;
		bool grabbed = false;
		bool hovered = false;
		bool localSpace = false;
		quat oldq;
		quat newq;
		bool flipOrt = false;

		void Build(float gimbalScale = 1.f)
		{
			model = *Assets::ImportModel("Assets\\hoop thin.fbx");
			float _scale = gimbalScale;
			mat4 scalem = Transform::Scale(_scale, _scale, _scale);
			float offsetd = 10.f;
			//moffset[0] = (Transform::Translate(10.f, offsetd, offsetd)) * Transform::GetEulerXYZ(0.f, 0.f, 90.f);
			//moffset[1] = (Transform::Translate(offsetd, 0.f, offsetd));
			//moffset[2] = (Transform::Translate(offsetd, offsetd, 10.f)) * Transform::GetEulerXYZ(-90.f, 0.f, 0.f);

			moffset[0] = Transform::GetEulerXYZ(0.f, 90.f, 0.f);
			moffset[1] = Transform::GetEulerXYZ(90.f, 00.f, 0.f);
			moffset[2] = Transform::GetEulerXYZ(0.f, 0.f, 0.f);

			coffset[0] = vec4(1.f, 0.f, 0.f, 1.f);
			coffset[1] = vec4(0.f, 1.f, 0.f, 1.f);
			coffset[2] = vec4(0.f, 0.f, 1.f, 1.f);

			id = 2;

			newq = transform.getRotation();

			localSpace = true;

			scale = gimbalScale;
		}

		void UpdateOffset(const vec3& rayStart, const vec3& rayDir,const vec2& mousePosition)
		{
			if (hitID == 0)
				return;

			if (hitID == 4)
			{
			}else if (hitID != 0 && !grabbed) {

				if (hitID == 1)
				{
					axisConstraint = vec3(1.f, 0.f, 0.f);
					if (localSpace)
						localAxisConstraint = transform.getRight();
				}
				if (hitID == 2)
				{
					axisConstraint = vec3(0.f, 1.f, 0.f);
					if (localSpace)
						localAxisConstraint = transform.getUp();
				}
				if (hitID == 3)
				{
					axisConstraint = vec3(0.f, 0.f, 1.f);
					if (localSpace)
						localAxisConstraint = transform.getForward();
				}
				if (hitID == 5)
				{
				}
				if (hitID == 6)
				{
				}
				if (hitID == 7)
				{
				}
			}

			if (!grabbed)
			{
				oldhit = clickPoint;
				//oldq = newq;
				oldq = transform.getRotation();
			}else{
				WorldSpaceHandle(rayStart, rayDir);
			}
		}

		void WorldSpaceHandle(const vec3 rayStart, const vec3 rayDir)
		{
			vec3 centre_of_rotation = transform.getPosition();
			flipOrt = false;
			if (!IntersectRayPlane(rayStart, rayDir, hitPoint))
				return;

			vec3 n = axisConstraint;
			if (localSpace)
				n = localAxisConstraint;

			vec3 delta;
			if(flipOrt)
				delta = glm::normalize(centre_of_rotation - hitPoint);
			else
				delta = glm::normalize(hitPoint - centre_of_rotation);

			const vec3 start_delta = glm::normalize(oldhit - centre_of_rotation);
			
			const vec3 side = glm::normalize(glm::cross(n, start_delta));
			const float y = glm::clamp(glm::dot(delta, start_delta), -1.0f, 1.0f);
			const float x = glm::clamp(glm::dot(delta, side), -1.0f, 1.0f);

			float theta = atan2f(x,y);
			quat q = glm::normalize(glm::angleAxis(theta, n));

			newq = glm::normalize(q*oldq);
			if (localSpace)
				transform.setRotation(newq);
		}

		bool IntersectRayPlane(const vec3& rayOrigin, const vec3& rayDir, vec3& hit_t)
		{
			vec4 plane = vec4(localAxisConstraint, -dot(localAxisConstraint, oldhit));

			float denom = glm::dot(vec3(plane), rayDir);
			if (std::abs(denom) == 0) 
				return false;

			float _t = -dot(plane, vec4(rayOrigin, 1)) / denom;
			if (_t < 0.0f)
				flipOrt = true;

			hit_t = rayOrigin + rayDir *  _t;
			return true;
		}

		//extract
		bool RayIntersectsPlane(const vec3& normal, const vec3& planePoint, const vec3& rayVector, const vec3 rayOrigin)
		{
			double denom = glm::dot(normal, rayVector);
			if (denom < 1E-6)
			{
				vec3 rayPlaneVector = planePoint - rayOrigin;
				double intersectionCheck = glm::dot(rayPlaneVector, normal) / denom;
				return intersectionCheck >= 0;
			}
			return false;
		}
		//extract
		vec3 ProjectPointOnPlane(const vec3& planeNormal, const vec3& planePoint, const vec3& point) {

			float distance;
			//First calculate the distance from the point to the plane:
			distance = glm::dot(planeNormal, (point - planePoint));

			//Reverse the sign of the distance
			distance *= -1;
			return point +  (planeNormal* distance);
		}
	};
}

