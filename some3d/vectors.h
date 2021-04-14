#pragma once
#include <math.h>
#include <SFML/Graphics.hpp>

class vec2 {
public:
	double coords[3];
	double & x = coords[0], & y = coords[1];
	vec2(double x, double y) {
		coords[0] = x;
		coords[1] = y;
	}
	vec2(vec2 const & p) {
		(*this) = p;
	}
	double & operator[](int idx) {
		return coords[idx];
	}
	double operator[](int idx) const {
		return coords[idx];
	}
	void operator=(vec2 const & p) {
		coords[0] = p[0];
		coords[1] = p[1];
	}
};

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
	vec3(double x_) {
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
	vec3 operator*(double s) const {
		return vec3(x * s, y * s, z * s);
	}
	vec3 operator+(vec3 const & v) const {
		return vec3(x + v.x, y + v.y, z + v.z);
	}
	vec3 operator-(vec3 const & v) const {
		return *this + v*-1;
	}
	vec3 operator/(double s) const{
		return (*this) * (1 / s);
	}
	sf::Vector3f uniform() {
		return sf::Vector3f(x, y, z);
	}
};

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
	vec3 operator*(vec3 const & r) {
		return vec3(dot(mat[0], r), dot(mat[1], r), dot(mat[2], r));
	}
	mat3 operator*(mat3 const & other) {
		mat3 other_t = other.T();
		mat3 res;
		for (int i = 0; i < 3; i += 1) {
			for (int j = 0; j < 3; j += 1) {
				res.mat[i][j] = dot(mat[i], other_t.mat[j]);
			}
		}
		return res;
	}
	mat3 T() const {
		return mat3(vec3(m1.x, m2.x, m3.x), vec3(m1.y, m2.y, m3.y), vec3(m1.z, m2.z, m3.z));
	}

	void operator=(mat3 const & other) {
		m1 = other.m1;
		m2 = other.m2;
		m3 = other.m3;
	}
};

mat3 rot_oz(double t) {
	return mat3({cos(t), -sin(t), 0}, {sin(t), cos(t), 0}, {0,0,1});
}
mat3 rot_ox(double t) {
	return mat3({1,0,0}, {0, cos(t), -sin(t)}, {0, sin(t), cos(t)});
}
mat3 rot_oy(double t) {
	return mat3({cos(t), 0, sin(t)}, {0, 1, 0}, {-sin(t), 0, cos(t)});
}