#include "vector.h"

vec3::vec3(float X, float Y, float Z)
{
	x = X; y = Y; z = Z;
}

float& vec3::operator[](int index)
{
	switch (index) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	}
}