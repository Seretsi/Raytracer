#include "Transform.h"

/*
	Accept normalized axis and rotate
*/
mat3 Transform::rotate(float degrees, vec3& axis)
{
	// rodriguez formula
	float rads = degrees * glm::pi<float>() / 180.0f;
	mat3 ret = mat3(1.0f);
	ret *= cos(rads);

	mat3 aa = mat3(axis.x * axis.x, axis.x * axis.y, axis.x * axis.z,
		axis.x * axis.y, axis.y * axis.y, axis.y * axis.z,
		axis.x * axis.y, axis.y * axis.z, axis.z * axis.z);
	ret += (1 - cos(rads)) * aa;

	mat3 a_dual = mat3(0.0f, axis.z, -axis.y,
		-axis.z, 0.0f, axis.x,
		axis.y, -axis.x, 0.0f);
	ret += a_dual;

	return ret;
}

mat4 Transform::lookat(vec3& eye, vec3& center, vec3& up)
{
	//computer coordinate system
	vec3 forward = glm::normalize(eye - center);
	vec3 right = glm::normalize(glm::cross(up, forward));
	vec3 properUp = glm::normalize(glm::cross(forward, right));
	vec3 e = vec3(-glm::dot(eye, right), -glm::dot(eye, properUp), -glm::dot(eye, forward));

	mat4 ret = mat4(right.x, properUp.x, forward.x, 0.0f,
		right.y, properUp.y, forward.y, 0.0f,
		right.z, properUp.z, forward.z, 0.0f,
		e.x, e.y, e.z, 1.0f);
	return ret;
}