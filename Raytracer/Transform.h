#pragma once
//#include "vector.h"
//#include "matrix.h"
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;

class Transform
{
public:
	static mat3 rotate(float degrees, vec3& axis);
	
	static mat4 lookat(vec3& eye, vec3& center, vec3& up);
};