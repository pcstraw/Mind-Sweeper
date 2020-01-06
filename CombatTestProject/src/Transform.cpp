#include "Transform.h"

using nativenamespace;

void Transform::setPosition(const glm::vec3& value)
{
	WorldPosition = value;

	if (Parent)
	{
		//if parented, set the local position
		//relative to the parent
		glm::quat parentSpaceConjugate = glm::conjugate(Parent->WorldRotation);

		LocalPosition = (parentSpaceConjugate *
			(value - Parent->WorldPosition)) /
			Parent->WorldScale;
	}
}

//world position
glm::vec3 Transform::getPosition()
{
	if (Parent != NULL)
	{
		//The world position is calculated relative
		//to the parent
		WorldPosition = Parent->WorldPosition +
			Parent->WorldRotation * (LocalPosition * Parent->WorldScale);
		return WorldPosition;

	}
	else {
		return WorldPosition;
	}
}

//haven't tested this yet
glm::quat Transform::SetDirection(const glm::vec3& axis, const glm::vec3& direction)
{
	glm::quat q;
	glm::vec3 a = cross(axis, direction);
	q.x = a.x;
	q.y = a.y;
	q.z = a.z;
	q.w = sqrt((pow(glm::length(axis), 2)) * (pow(glm::length(direction), 2))) + dot(axis, direction);
	return glm::normalize(q);
}

void Transform::setEulerXYZ(const vec3& eulerAngles)
{
	setEuler(eulerAngles.x, eulerAngles.z, eulerAngles.y);
}

void Transform::setEulerXYZ(float Xrot, float Yrot, float Zrot)
{
	setEuler(Xrot,Zrot,Yrot);
}

void Transform::setEuler(const vec3& eulerAngles)
{
	setEuler(eulerAngles.x, eulerAngles.y, eulerAngles.z);
}

void Transform::setEuler(float Xrot,float Yrot,float Zrot)
{
	vec3 euler = vec3(glm::radians(Xrot), glm::radians(Yrot), glm::radians(Zrot));
	glm::mat3 rotm = glm::orientate3(euler);
	quat q = glm::toQuat(rotm);
	setRotation(q);
}

void Transform::setRotation(const glm::quat& value)
{
	WorldRotation = value;
	rotationMatrix = glm::toMat4(WorldRotation);
}

glm::quat Transform::getRotation()
{
	if (Parent)
	{
		WorldRotation = Parent->getRotation()*LocalRotation;
		return WorldRotation;
	}
	else
		return WorldRotation;
}

void Transform::setScale(const glm::vec3& value)
{
	if (Parent)
	{
		glm::quat parentSpaceConjugate = glm::conjugate(Parent->WorldRotation);
		LocalScale = parentSpaceConjugate*(value / Parent->WorldScale);
	}
	else {
		WorldScale = value;
	}
}

glm::vec3 Transform::getScale()
{
	if (Parent)
	{
		WorldScale = Parent->WorldScale*LocalScale;
		return WorldScale;
	}
	else {
		return WorldScale;
	}
}

void Transform::SetLocalPosition(const glm::vec3& value)
{
	LocalPosition = value;
}

glm::vec3 Transform::getLocalPosition()
{
	return LocalPosition;
}

void Transform::SetLocalRotation(const glm::quat& value)
{
	LocalRotation = value;
}

glm::quat Transform::getLocalRotation()
{
	return LocalRotation;
}

void Transform::SetLocalScale(const glm::vec3& value)
{
	LocalScale = value;
}

glm::vec3 Transform::getLocalScale()
{
	if (Parent)
		return LocalScale;
	else
		return WorldScale;
}

glm::mat4 Native::Transform::GetWorldMatrix()
{
	mat4 mats = glm::scale(mat4(1.f), getScale());
	mat4 matr = rotationMatrix;
	mat4 matt = glm::translate(mat4(1.f), getPosition());
	mat4 mat = matt * mats * matr;
	worldMatrix = mat;
	return mat;
}

glm::mat4 Transform::GetTranslationMatrix()
{
	return glm::translate(glm::mat4(1.f), getPosition());
}

glm::mat4 Transform::GetScaleMatrix()
{
	return glm::scale(glm::mat4(1.f), getScale());
}

glm::mat4 Transform::GetRotationMatrix()
{
	return rotationMatrix;
}