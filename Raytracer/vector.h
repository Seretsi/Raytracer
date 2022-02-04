#pragma once
class vec3
{
public:
	float x, y, z;

	vec3() { x = y = z = 0.0f; }
	vec3(float X, float Y, float Z);
	
	float& operator[](int index);
};

