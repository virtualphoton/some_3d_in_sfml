#pragma once
#include <math.h>
#include <SFML/Graphics.hpp>


#define VEC_OP(O, O_EQ) \
vec3 operator O(vec3 const& v0, vec3 const & v) { \
	return vec3(v0.x O v.x, v0.y O v.y, v0.z O v.z);\
}\
vec3 operator O(vec3 const& v0, double d){ \
	return v0 O vec3(d);\
}\
vec3 operator O(double d, vec3 const& v0){ \
	return vec3(d) O v0;\
}\
template <class T>\
void operator O_EQ(vec3 & self, T const & v){ \
	self = self O v;\
}

#define MAT_OP(O, O_EQ)\
mat3 operator O(mat3 const & mat0, mat3 const & other){\
	return mat3(mat0.m1 O other.m1, mat0.m2 O other.m2, mat0.m3 O other.m3);\
}\
mat3 operator O(mat3 const & mat0, vec3 const & other){\
	return mat3(mat0.m1 O other, mat0.m2 O other, mat0.m3 O other);\
}\
mat3 operator O(vec3 const & other, mat3 const mat0) {\
		return mat3(other O mat0.m1, other O mat0.m2, other O mat0.m3); \
}\
mat3 operator O(mat3 const & mat0, double other){\
	return mat0 O vec3(other);\
}\
mat3 operator O(double other, mat3 const & mat0){\
	return vec3(other) O mat0;\
}\
template <class T>\
void operator O_EQ(mat3 & self, T const & other){\
	self = self O other;\
}

class mat3;

class vec3 {
public:
	double coords[3];
	double & x = coords[0], & y = coords[1], & z = coords[2];
	vec3() {}
	vec3(double * const & arr) {
		vec3(arr[0], arr[1], arr[2]);
	}
	vec3(double x_, double y_, double z_) {
		x = x_;
		y = y_;
		z = z_;
	}
	explicit vec3(double x_) {
		x = y = z = x_;
	}
	vec3(vec3 const & p) {
		(*this) = p;
	}
	double & operator[](int idx) {
		return coords[idx];
	}
	double operator[](int idx) const{
		return coords[idx];
	}
	void operator=(vec3 const &  p) {
		x = p.x;
		y = p.y;
		z = p.z;
	}

	sf::Vector3f uniform() {
		return sf::Vector3f(x, y, z);
	}
};

VEC_OP(+, +=);
VEC_OP(-, -=);
VEC_OP(*, *=);
VEC_OP(/, /=);

vec3 cross(vec3 const & v1, vec3 const & v2) {
	return vec3(v1.y*v2.z - v1.z*v2.y, -v1.x * v2.z + v1.z * v2.x, v1.x * v2.y - v1.y * v2.x);
}
double dot(vec3 const & v1, vec3 const & v2) {
	return v1.x * v2.x + v1.y*v2.y + v1.z*v2.z;
}
double length(vec3 const & v) {
	return sqrt(dot(v, v));
}
vec3 normalize(vec3 const & v) {
	return v / length(v);
}

vec3 & operator-=(vec3 & v, vec3 const & other) {
	v = vec3(v.x + other.x, v.y + other.y, v.z + other.z);
	return v;
}

class mat3 {
public:
	vec3 mat[3];
	vec3 & m1 = mat[0], & m2 = mat[1], & m3 = mat[2];
	mat3() {}

	mat3(vec3 const & r1, vec3 const & r2, vec3 const & r3) {
		m1 = r1;
		m2 = r2;
		m3 = r3;
	}
	mat3(mat3 const & p) {
		(*this) = p;
	}

	void operator=(mat3 const & other) {
		m1 = other.m1;
		m2 = other.m2;
		m3 = other.m3;
	}

	mat3 operator*(double d) const {
		return mat3(m1 * d, m2 * d, m3 * d);
	}
	vec3 operator*(vec3 const & r) const {
		return vec3(dot(mat[0], r), dot(mat[1], r), dot(mat[2], r));
	}
	mat3 operator*(mat3 const & other)const {
		mat3 other_t = other.T();
		mat3 res;
		for (int i = 0; i < 3; i += 1)
			for (int j = 0; j < 3; j += 1)
				res.mat[i][j] = dot(this->mat[i], other_t.mat[j]);
		return res;
	}

	mat3 T() const {
		return mat3(vec3(m1.x, m2.x, m3.x), vec3(m1.y, m2.y, m3.y), vec3(m1.z, m2.z, m3.z));
	}
};

MAT_OP(+, +=);
MAT_OP(-, -=);

vec3 operator*(vec3 const & v, mat3 const & m){
	return m.T() * v;
}

mat3 I = mat3({1, 0, 0}, {0, 1, 0}, {0, 0, 1});

mat3 cross_repr(vec3 v) {
	return mat3({0, -v.z, v.y}, {v.z, 0, -v.x}, {-v.y, v.x, 0});
}
mat3 rot(vec3 axis, double phi) {
	mat3 w = cross_repr(axis);
	return I + w*sin(phi) + w*w*(1-cos(phi));
}

