#pragma once
#include "NativeHeader.h"
#include "Vertex.h"
#include "glm\gtx\vector_angle.hpp"

nativenamespace{

	class Face
	{
	public:
		Face()
		{
		}
		
		Face(const Face& f)
		{
			points[0] = f.points[0];
			points[1] = f.points[1];
			points[2] = f.points[2];
			normal = f.normal;
			index = f.index;
			modelID = f.modelID;
			area = f.area;
		}
		
		Face(Vertex* A,Vertex* B,Vertex* C)
		{
			points[0] = *A;
			points[1] = *B;
			points[2] = *C;
		}

		Face(const Vertex& A,const Vertex& B,const Vertex& C)
		{
			points[0] = A;
			points[1] = B;
			points[2] = C;
			ComputeNormal();
			points[0].faceNormal = normal;
			points[1].faceNormal = normal;
			points[2].faceNormal = normal;
		}

		Face(Vertex* A,Vertex* B,Vertex* C,vec3 Normal)
		{
			points[0] = *A;
			points[1] = *B;
			points[2] = *C;
			normal = Normal;
		}

		Face(const vec3& a,const vec3& b,const vec3& c)
		{
			points[0] = Vertex(a);
			points[1] = Vertex(b);
			points[2] = Vertex(c);
			ComputeNormal();
			points[0].faceNormal = normal;
			points[1].faceNormal = normal;
			points[2].faceNormal = normal;
		}

		Face(const vec3& a,const vec3& b,const vec3& c,const vec3& n)
		{
			points[0] = Vertex(a);
			points[1] = Vertex(b);
			points[2] = Vertex(c);
			normal = n;
			points[0].faceNormal = normal;
			points[1].faceNormal = normal;
			points[2].faceNormal = normal;
		}

		Vertex points[3];
		glm::vec3 normal;
		glm::vec3 worldNormal;
		glm::mat4 matrixForm;
		glm::uvec3 faceIndices;
		unsigned int index;
		float area;
		int modelID;

		glm::vec3 ComputeNormal()
		{
			vec3 v0 = points[0].position - points[1].position;
			vec3 v1 = points[1].position - points[2].position;
			vec3 n = cross(v0, v1);
			normal = normalize(n);
			return normal;
		}

		glm::mat3 AsMat3()
		{
			mat3 mat;
			for (int i = 0; i < 3; i++)
			{
				mat[i] = points[i].position;
			}
			return mat;
		}

		static glm::vec3 GetNormal(const vec3& a, const vec3& b, const vec3& c)
		{
			vec3 v0 = b - a;
			vec3 v1 = c - a;
			vec3 n = cross(v0, v1);
			return normalize(n);
		}
		//https://stackoverflow.com/questions/3706331/how-to-rotate-a-3d-tringle-flat-against-the-wall
		static quat align(const vec3& v1, const vec3& v2)
		{
			vec3 bisector = normalize(v1 + v2);
			double cosHalfAngle = dot(v1,bisector);
			vec3 c;

			if (cosHalfAngle == 0.0) {
				c = cross(v1,bisector);
			}
			else {
				c = normalize(cross(v1,vec3(v2.z, v2.x, v2.y)));
			}

			return quat(cosHalfAngle, c.x, c.y, c.z);
		}

		quat alignVecs(const vec3& vec1, const vec3& vec2)
		{
			vec3 LocalW(cross(vec1, vec2));
			float Dot = dot(vec1,vec2);
			quat q(1 + Dot, LocalW.x, LocalW.y, LocalW.z);

			return normalize(q);
		}

		void AlignFaces()
		{
			vec3 v = normal;
			vec3 axis = vec3(0, -1, 0);
			quat Q = align( v, axis);
			vec3 a = points[0].position;
			for (int i = 0; i < 3; i++)
			{
				/*
				vec3 p = points[i].position;
				quat q = quat(0, p.x, p.y, p.z);
				quat qr = Q * q * inverse(Q);
				vec3 p2 = vec3(qr.x, qr.y, qr.z);
				*/
				//points[i].position = points[i].position - a;
				//points[i].position = quat(v,axis)*points[i].position;
				//points[i].position = alignVecs(v, axis)*points[i].position;
				//points[i].position += a;
				points[i].position.y = 0.f;
			}
		}

		vec4 GetPlane()
		{
			//distance from plane to the origin
			vec3 worldPoint = points[0].position;
			vec3 worldNormal = normal;
			float d = -dot(worldPoint, worldNormal); //origni is 0,0,0, so just negate the result
			return vec4(worldNormal, d);
		}

		quat fromtwovectors(vec3 u, vec3 v)
		{
			/*
			float norm_u_norm_v = sqrt(glm::length2(u) * glm::length2(v));
			vec3 w = cross(u, v);
			quat q = quat(norm_u_norm_v + dot(u, v), w.x, w.y, w.z);
			return normalize(q);
			*/
			float norm_u_norm_v = sqrt(dot(u, u) * dot(v, v));
			float real_part = norm_u_norm_v + dot(u, v);
			vec3 w;

			if (real_part < 1.e-6f * norm_u_norm_v)
			{
				/* If u and v are exactly opposite, rotate 180 degrees
				* around an arbitrary orthogonal axis. Axis normalisation
				* can happen later, when we normalise the quaternion. */
				real_part = 0.0f;
				w = abs(u.x) > abs(u.z) ? vec3(-u.y, u.x, 0.f)
					: vec3(0.f, -u.z, u.y);
			}
			else
			{
				/* Otherwise, build quaternion the standard way. */
				w = cross(u, v);
			}

			return normalize(quat(real_part, w.x, w.y, w.z));
		}

		static void project_triangle(
			const vec3& p0,
			const vec3& p1,
			const vec3& p2,
			vec2& z0,
			vec2& z1,
			vec2& z2
		) {
			vec3 X = p1 - p0;
			X = normalize(X); // normalized by dividing x,y,z with length of the vector
			vec3 Z = cross(X, (p2 - p0));
			Z = normalize(Z);
			vec3 Y = cross(Z, X);  //cross product
			const vec3& O = p0;

			double x0 = 0;
			double y0 = 0;
			double x1 = (p1 - O).length();
			double y1 = 0;
			double x2 = dot((p2 - O), X);
			double y2 = dot((p2 - O), Y);

			z0 = vec2(x0, y0);
			z1 = vec2(x1, y1);
			z2 = vec2(x2, y2);
		}

		void ProjectToUVSpace()
		{
			vec3 o = points[0].position;

			vec3 p1 = points[0].position - o;
			vec3 p2 = points[1].position - o;
			vec3 p3 = points[2].position - o;

			vec3 a = vec3(0, -1, 0);
			vec3 b = GetNormal(p1,p2,p3);
			vec3 right = normalize(p2 - p3);
			vec3 forward = normalize(cross(right, p3 - p2));
			vec3 up = normalize(cross(forward, right));
			quat q = inverse(fromtwovectors(a,b));

			points[0].position = q * (p1);
			points[1].position = q * (p2);
			points[2].position = q * (p3);


			vec3 ay = vec3(1,0,0);
			vec3 by = normalize(points[1].position - points[2].position);
			quat qy = inverse(fromtwovectors(right, by));
			points[0].position = qy * points[0].position;
			points[1].position = qy * points[1].position;
			points[2].position = qy * points[2].position;


			points[0].position += o;
			points[1].position += o;
			points[2].position += o;
			//points[0].position.y = 0;
			//points[1].position.y = 0;
			//points[2].position.y = 0;
			/*
			vec3 a = points[0].position;
			vec3 b = points[1].position;
			vec3 c = points[2].position;
			vec3 right = normalize(a - b);
			vec3 forward = normalize(cross(right, c - b));
			vec3 up = normalize(cross(forward,right));

			vec3 origin = vec3(0.f);
			for (int i = 0; i < 3; i++)
			{
				int x = dot(points[i].position - origin, right);
				int y = dot(points[i].position - origin, up);
				int z = dot(points[i].position - origin, forward);
				points[i].position = vec3(x, y, z);
			}
			*/
			/*
			glm::vec3 v = glm::cross(b, a);
			float angle = acos(glm::dot(b, a) / (glm::length(b) * glm::length(a)));
			glm::mat4 rotmat = glm::rotate(angle, v);
			//	mat4 r = transpose(glm::lookAt(points[0].position,normal,vec3(0,0,-1)));
			points[0].position = vec3(rotmat*vec4(points[0].position, 1));
			points[1].position = vec3(rotmat*vec4(points[1].position, 1));
			points[2].position = vec3(rotmat*vec4(points[2].position, 1));
			//points[0].position.y = 0;
			//points[1].position.y = 0;
			//points[2].position.y = 0;
			*/
			/*
			vec3 a = vec3(0, 0, 1);
			vec3 b = n;
			quat q = inverse(fromtwovectors(a,b));
			
			points[0].position = q * (points[0].position);
			points[1].position = q * (points[1].position);
			points[2].position = q * (points[2].position);

			points[0].position += p1;
			points[1].position += p2;
			points[2].position += p3;
			*/
			//points[0].position *=vec3(1,0,1);
			//points[1].position *=vec3(1,0,1);
			//points[2].position *=vec3(1,0,1);
		}

		void TestMatrixCompo()
		{
			vec3 p1 = points[0].position - points[0].position;
			vec3 p2 = points[1].position - points[0].position;
			vec3 p3 = points[2].position - points[0].position;

			vec3 v0 = p2 - p1;
			vec3 v1 = p3 - p1;
			vec3 n = cross(v0, v1);
			n = normalize(n);

			float nroot = sqrt(n.x*n.x + n.y*n.y);
			mat3 rot;
			rot[0][0] = n.x / nroot;
			rot[0][1] = n.y / nroot;
			rot[0][2] = 0.f;
			rot[1][0] = -n.y / nroot;
			rot[1][1] = n.x / nroot;
			rot[1][2] = 0.f;
			rot[2][0] = 0.f;
			rot[2][1] = 0.f;
			rot[2][2] = 1.f;

			vec3 nn = rot * n;

			mat3 roty;
			roty[0][0] = nn.z;
			roty[0][1] = 0.f;
			roty[0][2] = -nn.x;
			roty[1][0] = 0.f;
			roty[1][1] = 1.f;
			roty[1][2] = 0.f;
			roty[2][0] = nn.x;
			roty[2][1] = 0.f;
			roty[2][2] = nn.z;

			points[0].position = rot * points[0].position;
			points[1].position = rot * points[1].position;
			points[2].position = rot * points[2].position;

			points[0].position += points[0].position;
			points[1].position += points[0].position;
			points[2].position += points[0].position;
		}

		void ComputeArea()
		{
			float a = glm::length(points[0].position);
			float b = glm::length(points[1].position);
			float c = glm::length(points[2].position);
			float s = (a + b + c)*0.5f;
			area = sqrtf(s * (s - a)*(s - b)*(s - c));
		}

		void ComputeWeightedNormals()
		{
			ComputeArea();

			float angleA = glm::angle(points[0].position - points[1].position, points[0].position - points[2].position);
			points[0].weightedNormal = normal*area  * angleA;

			float angleB = glm::angle(points[1].position - points[0].position, points[1].position - points[2].position);
			points[1].weightedNormal = normal*area  * angleB;

			float angleC = glm::angle(points[2].position - points[0].position, points[2].position - points[1].position);
			points[2].weightedNormal = normal*area  * angleC;
		}

		static std::vector<unsigned int> ComputeIndices(std::vector<Face>& faces)
		{
			unsigned int index = 0;
			std::vector<unsigned int> indices;
			for (Face& f : faces)
			{
				indices.push_back(index);
				f.points[0].index = index;
				index++;
				indices.push_back(index);
				f.points[1].index = index;
				index++;
				indices.push_back(index);
				f.points[2].index = index;
				index++;
			}
			return indices;
		}

		static void GetData(std::vector<Face>& faces,std::vector<Vertex>& vertices,std::vector<unsigned int>& indices,bool updateIndices)
		{
			unsigned int index = 0;
			indices.clear();
			vertices.clear();
			for (Face& f : faces)
			{
				for (int i = 0; i < 3; i++)
				{
					if(updateIndices)
						f.points[i].index = index;
					vertices.push_back(f.points[i]);
					indices.push_back(f.points[i].index);
					if (updateIndices)
						index++;
				}
			}
		}

		void Debug()
		{
			cout << "Face:->" << endl;
			cout << "A: " << points[0].index << endl;
			cout << "B: " << points[1].index << endl;
			cout << "C: " << points[2].index << endl;
			cout << "--------------->" << endl;
		}


		static std::vector<Face> BuildTet()
		{
			vec3 p1 = vec3(0);
			vec3 p2 = vec3(128, 0, 0);
			vec3 p3 = vec3(64, 128, 0);
			vec3 p4 = vec3(64, 64, 128);

			Face fa(p1, p2, p4);
			Face fb(p2, p3, p4);
			Face fc(p3, p2, p1);
			Face fd(p4, p3, p1);

			std::vector<Face> tet;
			tet.push_back(fa);
			tet.push_back(fb);
			tet.push_back(fc);
			tet.push_back(fd);
			return tet;
		}
	};
}

