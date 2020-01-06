#pragma once
#include "NativeHeader.h"
#include "GPUMesh.h"
#include "Transform.h"

nativenamespace{

	class GimbalTranslation
	{

	public:

		GPUMesh gpuMesh;
		float scale = 1.f;
		float axisLength = 10.f;
		Transform transform;
		vec3 hitPoint = vec3(0);
		vec3 whitpoint = vec3(0);
		vec3 offset = vec3(0);
		vec3 oldhit = vec3(0);
		vec3 axisConstraint = vec3(0);
		unsigned int hitID = 0;
		bool hovered = false;
		bool grabbed = false;

		void Build(float GimbalScale = 1.f,float AxisLength = 10.f)
		{
			scale = GimbalScale;
			axisLength = AxisLength;
			BuildCube();
			gpuMesh.BufferMesh(points,indices);
		}

		void UpdateOffset(const vec3& rayStart, const vec3& rayDir)
		{
			if (hitID == 0)
				return;

			if (hitID == 4)
			{
				axisConstraint = -rayDir;
				ConstrainToPlane(axisConstraint, rayStart, rayDir);
				
			}else if(hitID != 0){

				if (hitID == 1)
				{
					axisConstraint = vec3(1, 0, 0);
					ConstrainToAxis(axisConstraint, rayStart, rayDir);
				}
				if (hitID == 2)
				{
					axisConstraint = vec3(0, 1, 0);
					ConstrainToAxis(axisConstraint, rayStart, rayDir);
				}
				if (hitID == 3)
				{
					axisConstraint = vec3(0, 0, 1);
					ConstrainToAxis(axisConstraint, rayStart, rayDir);
				}
				if (hitID == 5)
				{
					axisConstraint = vec3(0, 0, 1);
					ConstrainToPlane(axisConstraint, rayStart, rayDir);
				}
				if (hitID == 6)
				{
					axisConstraint = vec3(1, 0, 0);
					ConstrainToPlane(axisConstraint, rayStart, rayDir);
				}
				if (hitID == 7)
				{
					axisConstraint = vec3(0, 1, 0);
					ConstrainToPlane(axisConstraint, rayStart, rayDir);
				}
				
				//cout << "Grabbbed!" << endl;
			}

			offset = hitPoint - oldhit;
			oldhit = hitPoint;
			if (grabbed)
			{
				//	cout << "Offset: " << glmGetString(offset) << endl;
				vec3 _pos = transform.getPosition();
				//if (glm::length(offset) > 0.0001f)
				//	bool breakhere = true;
				transform.setPosition(_pos + offset);
			}
		//	cout << "Hit ID: " << hitID << endl;
		//	cout << "Hit Point: " << glmGetString(hitPoint) << endl;
		//	cout << "Old Hit  : " << glmGetString(oldhit) << endl;
		}

		void ConstrainToAxis(const vec3& n, const vec3& rayOrigin, const vec3& rayDir)
		{
			vec3 p1 = vec3(0);
			vec3 p2 = vec3(0);
			vec3 ln = n;
			vec3 p = vec3(0);
			if (LineLineIntersection(p,p + ln,rayOrigin, rayOrigin + rayDir,1.0f, p1, p2))
				hitPoint = p1;
		}

		void ConstrainToPlane(const vec3& n, const vec3& rayOrigin, const vec3& rayDir)
		{
			vec3 p = vec3(0);
			vec3 planep = whitpoint;
			if(RayPlane(n, rayOrigin, rayDir, planep, p))
				hitPoint = p;
		}

		bool RayPlane(const vec3& n,const vec3& rayOrigin, const vec3& rayDir,const vec3& planePoint,vec3& result)
		{
			float denom = glm::dot(n,rayDir);
			if (abs(denom) > 0.0001f) // your favorite epsilon
			{
				float t = glm::dot(planePoint - rayOrigin,n) / denom;
				if (t >= 0.1f)
				{
					result = rayOrigin + rayDir * t; // you might want to allow an epsilon here too
					return true;
				}
			}
			return false;
		}

		bool LineLineIntersection(const vec3& line1Point1, const vec3& line1Point2,
			const vec3& line2Point1, const vec3& line2Point2, float threshold, vec3& resultSegmentPoint1, vec3& resultSegmentPoint2)
		{
			// Algorithm is ported from the C algorithm of 
			// Paul Bourke at http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/
			resultSegmentPoint1 = vec3(0.f);
			resultSegmentPoint2 = vec3(0.f);

			vec3 pa1 = line1Point1;
			vec3 pa2 = line1Point2;
			vec3 pb1 = line2Point1;
			vec3 pb2 = line2Point2;
			vec3 pab1 = pa1 - pb1;
			vec3 pb21 = pb2 - pb1;

			float eps = 0.00000001f;
			float epsS = eps * eps;
			if (glm::dot(pb21, pb21) < epsS) {
				return false;
			}
			vec3 p21 = pa2 - pa1;
			if (glm::dot(p21, p21) < epsS) {
				return false;
			}

			double dotAB1andB21 = glm::dot(pab1, pb21);
			double dotB21andP21 = glm::dot(pb21, p21);
			double dotAB1andP21 = glm::dot(pab1, p21);
			double dotB21andB21 = glm::dot(pb21, pb21);
			double dotP21 = glm::dot(p21, p21);

			double denom = dotP21 * dotB21andB21 - dotB21andP21 * dotB21andP21;
			if (abs(denom) < eps) {
				return false;
			}
			double numer = dotAB1andB21 * dotB21andP21 - dotAB1andP21 * dotB21andB21;

			double mua = numer / denom;
			double mub = (dotAB1andB21 + dotB21andP21 * (mua)) / dotB21andB21;

			resultSegmentPoint1 = pa1 + (p21 * (float)mua);
			resultSegmentPoint2 = pb1 + (pb21 * (float)mub);
			return true;
		}

		bool LineLineIntersection(const vec3& line1Point1,
			const vec3& line2Point1, float threshold, vec3& resultSegmentPoint1, vec3& resultSegmentPoint2)
		{
			// Algorithm is ported from the C algorithm of 
			// Paul Bourke at http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/
			resultSegmentPoint1 = vec3(0.f);
			resultSegmentPoint2 = vec3(0.f);

			vec3 pa1 = line1Point1;
			vec3 pa2 = line1Point1 * 100.f;
			vec3 pb1 = line2Point1;
			vec3 pb2 = line2Point1 * 100.f;
			vec3 pab1 = pa1 - pb1;
			vec3 pb21 = pb2 - pb1;

			float eps = 0.00000001f;
			float epsS = eps * eps;
			if (glm::dot(pb21, pb21) < epsS) {
				return false;
			}
			vec3 p21 = pa2 - pa1;
			if (glm::dot(p21, p21) < epsS) {
				return false;
			}

			double dotAB1andB21 = glm::dot(pab1, pb21);
			double dotB21andP21 = glm::dot(pb21, p21);
			double dotAB1andP21 = glm::dot(pab1, p21);
			double dotB21andB21 = glm::dot(pb21, pb21);
			double dotP21 = glm::dot(p21, p21);

			double denom = dotP21 * dotB21andB21 - dotB21andP21 * dotB21andP21;
			if (abs(denom) < eps) {
				return false;
			}
			double numer = dotAB1andB21 * dotB21andP21 - dotAB1andP21 * dotB21andB21;

			double mua = numer / denom;
			double mub = (dotAB1andB21 + dotB21andP21 * (mua)) / dotB21andB21;

			resultSegmentPoint1 = pa1 + (p21 * (float)mua);
			resultSegmentPoint2 = pb1 + (pb21 * (float)mub);

			if (glm::dot(resultSegmentPoint1, resultSegmentPoint2) > threshold*threshold)
				return false;

			return true;
		}

		void GetAABBXAxis(vec3& min, vec3& max)
		{
			min = vec3(0);
			max = vec3(0);
			for (const vec3& point : points)
			{
				vec3 p = point * vec3(axisLength*scale, scale, scale);
				min = glm::min(min, p);
				max = glm::max(max, p);
			}
		}

		void GetAABBYAxis(vec3& min, vec3& max)
		{
			min = vec3(0);
			max = vec3(0);
			for (const vec3& point : points)
			{
				vec3 p = point * vec3(scale, axisLength*scale, scale);
				min = glm::min(min, p);
				max = glm::max(max, p);
			}
		}

		void GetAABBZAxis(vec3& min, vec3& max)
		{
			min = vec3(0);
			max = vec3(0);
			for (const vec3& point : points)
			{
				vec3 p = point * vec3(scale, scale, axisLength*scale);
				min = glm::min(min, p);
				max = glm::max(max, p);
			}
		}

		ivec4 TestGimbalIntersection(const vec3& rayOrigin, const vec3& rayDir, float& intersectionDistance)
		{
			vec3 aabbmin = vec3(0);
			vec3 aabbmax = vec3(0);

			hovered = false;
			ivec4 intersections = ivec4(0);
			vec3 dist = vec3(10000.f);
			GetAABBXAxis(aabbmin, aabbmax);
			RayOBBSimple(rayOrigin, rayDir, aabbmin, aabbmax, 0.f, 10000.f, dist.x);

			GetAABBYAxis(aabbmin, aabbmax);
			RayOBBSimple(rayOrigin, rayDir, aabbmin, aabbmax, 0.f, 10000.f, dist.y);

			GetAABBZAxis(aabbmin, aabbmax);
			RayOBBSimple(rayOrigin, rayDir, aabbmin, aabbmax, 0.f, 10000.f, dist.z);

			if (dist.x < dist.y && dist.x < dist.z)
				return intersections = ivec4(1, 0, 0, 0);
			else if (dist.y < dist.x && dist.y < dist.z)
				return intersections = ivec4(0, 1, 0, 0);
			else if (dist.z < dist.x && dist.z < dist.y)
				return intersections = ivec4(0, 0, 1, 0);

			return intersections;
		}

		float Dist3D_Line_to_Line(const vec3& L11, const vec3& L12, const vec3& L21, const vec3& L22)
		{
			vec3   u = L12 - L11;
			vec3   v = L22 - L21;
			vec3   w = L11 - L21;
			float    a = dot(u, u);         // always >= 0
			float    b = dot(u, v);
			float    c = dot(v, v);         // always >= 0
			float    d = dot(u, w);
			float    e = dot(v, w);
			float    D = a * c - b * b;        // always >= 0
			float    sc, tc;

			float SMALL_NUM = 0.00001f;
			// compute the line parameters of the two closest points
			if (D < SMALL_NUM) {          // the lines are almost parallel
				sc = 0.f;
				tc = (b>c ? d / b : e / c);    // use the largest denominator
			}
			else {
				sc = (b*e - c * d) / D;
				tc = (a*e - b * d) / D;
			}

			// get the difference of the two closest points
			vec3   dP = w + (sc * u) - (tc * v);  // =  L1(sc) - L2(tc)
			return glm::length(dP);   // return the closest distance
		}

		//https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
		bool RayOBBSimple(const vec3& rayOrigin, const vec3& rayDir, const vec3& aabbmin, const vec3& aabbmax, float tmin, float tmax, float& dist) {

			mat4 worldinv = glm::inverse(transform.worldMatrix);
			vec3 _re = vec3(worldinv*vec4(rayOrigin + rayDir * tmax, 1.0f));
			vec3 _ro = vec3(worldinv*vec4(rayOrigin, 1.0f));
			vec3 _rd = glm::normalize(_re - _ro);
			for (int a = 0; a < 3; ++a) {
				float invD = 1.0f / _rd[a];
				float t0 = (aabbmin[a] - _ro[a]) * invD;
				float t1 = (aabbmax[a] - _ro[a]) * invD;
				if (invD < 0.0f) {
					float temp = t1;
					t1 = t0;
					t0 = temp;
				}

				tmin = t0 > tmin ? t0 : tmin;
				tmax = t1 < tmax ? t1 : tmax;

				if (tmax <= tmin)
					return false;
			}
			hovered = true;
			vec3 _hp = _ro + _rd * tmax;
			dist = tmax;
			return true;
		}

	private:

		void EmitTriangle(unsigned int a, unsigned int b, unsigned int c)
		{
			indices.push_back(uvec3(a, b, c));
		}

		void BuildCube()
		{
			float y = 1.0;
			vec3 origin = vec3(0.0);
			vec3 atop = origin + vec3(0.0, y, 0.0);
			vec3 btop = origin + vec3(0.0, y, 1.0);
			vec3 ctop = origin + vec3(1.0, y, 1.0);
			vec3 dtop = origin + vec3(1.0, y, 0.0);
			y = 0.0;
			vec3 abottom = origin + vec3(0.0, y, 0.0);
			vec3 bbottom = origin + vec3(0.0, y, 1.0);
			vec3 cbottom = origin + vec3(1.0, y, 1.0);
			vec3 dbottom = origin + vec3(1.0, y, 0.0);

			points.clear();
			points.push_back(atop); //0
			points.push_back(btop); //1
			points.push_back(ctop); //2
			points.push_back(dtop); //3
			points.push_back(abottom); //4
			points.push_back(bbottom); //5
			points.push_back(cbottom); //6
			points.push_back(dbottom); //7

			unsigned int at = 0;
			unsigned int bt = 1;
			unsigned int ct = 2;
			unsigned int dt = 3;
			unsigned int ab = 4;
			unsigned int bb = 5;
			unsigned int cb = 6;
			unsigned int db = 7;

			indices.clear();
			//top
			EmitTriangle(at, bt, ct);
			EmitTriangle(ct, dt, at);

			//bottom
			EmitTriangle(cb, bb, ab);
			EmitTriangle(ab, db, cb);

			//left x
			EmitTriangle(at, ab, bb);
			EmitTriangle(bb, bt, at);

			//front z
			EmitTriangle(bt, bb, cb);
			EmitTriangle(cb, ct, bt);

			//right x
			EmitTriangle(ct, cb, db);
			EmitTriangle(db, dt, ct);

			//back z
			EmitTriangle(dt, db, ab);
			EmitTriangle(ab, at, dt);

			//top
			/*
			EmitTriangle(0, 1, 2);
			EmitTriangle(2, 3, 0);
			//bottom
			EmitTriangle(6, 5, 4);
			EmitTriangle(4, 7, 6);
			//left x
			EmitTriangle(0, 4, 5);
			EmitTriangle(5, 1, 0);
			//front z
			EmitTriangle(1, 5, 6);
			EmitTriangle(6, 2, 1);
			//right x
			EmitTriangle(2, 6, 7);
			EmitTriangle(7, 3, 2);
			//back z
			EmitTriangle(3, 7, 4);
			EmitTriangle(4, 0, 3);
			*/
		}

		std::vector<vec3> points;
		std::vector<uvec3> indices;

	};

}
