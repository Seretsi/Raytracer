#pragma once
class mat3
{
public:
	float a, d, g,
		b, e, h,
		c, f, i;

	mat3() {};
	mat3(float _a, float _b, float _c, float _d, float _e, float _f, float _g, float _h, float _i);
};

class mat4
{
public:
	float a, e, i, m,
		b, f, j, n,
		c, g, k, o,
		d,  h, l, p;

	mat4() {
		a = e = i = m = b = f = j = n = c = g = k = o = d = h = l = p = 0.0f;
	};
	mat4(float _a, float _b, float _c, float _d, float _e, float _f, float _g, float _h, float _i, float _j, float _k, float _l, float _m, float _n, float _o, float _p);
};

