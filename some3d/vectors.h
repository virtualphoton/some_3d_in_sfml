#pragma once
#include <math.h>
#include <SFML/Graphics.hpp>
#include <string>
#include "useful_funcs.h"
#include "uniforms.h"

// this will be used to write shortly +, -, * and / operators for vectors (O and O_EQ are from same pair, e.g. +, +=)
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
vec3 & operator O_EQ(vec3 & self, T const & v){ \
	self = self O v;\
	return self;\
}

// same as above but for 3x3 matrices
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
mat3 & operator O_EQ(mat3 & self, T const & other){\
	self = self O other;\
	return self;\
}

class mat3;

/* class for 3d vector.
* access to coordinates via x, y, z or via []
* constructors:
*	- from 3 numbers or initializer_list with 3 elements (e.g. vec3({1, 2, 3}))
*	- from one number(then x = y = z = that number)
*	- from sf::Color. Conversion is opposite of color() method(below)
* conversions:
*	to bool -> false if every coord < epsilon (i.e. null-vector)
* methods:
*	uniform() -> return uniform that can be passed to glsl shader
*	color()   -> converts to sf::Color by scaling each coord from [0;1], double to [0;255], int. If coord is not in interval, corresponding bound is chosen
*	color_over(new_color, lambda) -> gives color created from (lambda*100) % of this vector color and (100-lambda*100)% of new_color
*   +=, -=, *=, /=  -> element-wise operations
* related methods:
*	cross(vec, vec) -> cross product
*	dot(vec, vec)   -> dot product
*	length(vec)     -> length, surprisingly
*	normalize(vec)  -> vec in same direction but of length 1. if length of vec is 0, return null-vector
*	+, -, *, /      -> element-wise operations
*/
class vec3 : public PossibleUniformData<sf::Glsl::Vec3> {
public:
	double epsilon = 1.e-6; // for checking if vector is null-vector
	double coords[3]{ 0, 0, 0 };
	double& x = coords[0], & y = coords[1], & z = coords[2];
	vec3() {
		glsl_type = "vec3";
	}

	vec3(double x, double y, double z) :vec3() {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	explicit vec3(double x_) :vec3() {
		x = y = z = x_;
	}
	vec3(vec3 const& p) :vec3() {
		(*this) = p;
	}
	explicit vec3(sf::Color const& p) :vec3() {
		x = p.r / 255.;
		y = p.g / 255.;
		z = p.b / 255.;
	}
	operator bool() const {
		return abs(x) > epsilon or abs(y) > epsilon or abs(z) > epsilon;
	}
	double& operator[](int idx) {
		return coords[idx];
	}
	double operator[](int idx) const {
		return coords[idx];
	}
	void operator=(vec3 const& p) {
		x = p.x;
		y = p.y;
		z = p.z;
	}
	sf::Glsl::Vec3 uniform() const override {
		return sf::Glsl::Vec3(x, y, z);
	}
	sf::Color color() const {
		return sf::Color(int(clench(x, 0, 1, 0, 255)), int(clench(y, 0, 1, 0, 255)), int(clench(z, 0, 1, 0, 255)));
	}
	sf::Color color_over(sf::Color const& px_color, double lambda) const;
};

VEC_OP(+, +=);
VEC_OP(-, -=);
VEC_OP(*, *=);
VEC_OP(/ , /=);

sf::Color vec3::color_over(sf::Color const& px_color, double lambda) const {
	lambda = clamp(lambda, 0, 1);
	vec3 px_col = vec3(px_color);
	return ((*this) * lambda + px_col * (1 - lambda)).color();
}

vec3 cross(vec3 const& v1, vec3 const& v2) {
	return vec3(v1.y * v2.z - v1.z * v2.y, -v1.x * v2.z + v1.z * v2.x, v1.x * v2.y - v1.y * v2.x);
}
double dot(vec3 const& v1, vec3 const& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
double length(vec3 const& v) {
	return sqrt(dot(v, v));
}
vec3 normalize(vec3 const& v) {
	if (not v)
		return v;
	return v / length(v);
}
/* crippled brother of vec3.
*	has additional 'w' coord(4th)
* methods(! only those. None others are defined(because vec3 is actually sufficient for maths and other things)):
*	uniform() -> to uniform to be passed into glsl shader
*	xyz()     -> returns vec3 from x, y and z of this vec4
*/
class vec4 : public PossibleUniformData<sf::Glsl::Vec4> {
public:
	std::string glsl_type = "vec4"; // for uniforms
	double coords[4]{ 0,0,0,0 };
	double& x = coords[0], & y = coords[1], & z = coords[2], & w = coords[3];
	vec4() {
		glsl_type = "vec4";
	}
	vec4(double x_, double y_, double z_, double w_) :vec4() {
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}
	explicit vec4(double x_) :vec4() {
		x = y = z = w = x_;
	}
	vec4(vec4 const& p) :vec4() {
		(*this) = p;
	}
	double& operator[](int idx) {
		return coords[idx];
	}
	double operator[](int idx) const {
		return coords[idx];
	}
	void operator=(vec4 const& p) {
		x = p.x;
		y = p.y;
		z = p.z;
		w = p.w;
	}
	sf::Glsl::Vec4 uniform() const override {
		return sf::Glsl::Vec4(x, y, z, w);
	}
	vec3 xyz() {
		return vec3(x, y, z);
	}
};

/* class for 3x3 matrices
* its rows are vec3 and accessed via m1, m2, m3 or via []
* constructors:
*	- from 3 vectors(row)
* methods:
*	T()    -> transpose of a matrix
*   +=, -= -> element-wise operations
*	*, *= :
*		element wise, if multiplied by double
*		matrix multiplication if multiplied by mat3 or vec3
* related constants:
*	I      -> identity matrix
* related methods:
*	cross_repr(vec3)    -> cross-product matrx of vector(in cross-product, if one vector is replaced by this matrix, then matrix product will be equal to cross product)
*	rot(vec3 axis, phi) -> returns rotation matrix. Multiplication is same as rotating against axis by vector phi(in positive direction(counterclockwise)):
*		rotated_vector = rotation_matrix*vector
*	*, *=  - same as for method, but passed multiplier is on the left
*/
class mat3 {
public:
	vec3 mat[3];
	vec3& m1 = mat[0], & m2 = mat[1], & m3 = mat[2];
	mat3() {}

	mat3(vec3 const& r1, vec3 const& r2, vec3 const& r3) {
		m1 = r1;
		m2 = r2;
		m3 = r3;
	}
	mat3(mat3 const& p) {
		(*this) = p;
	}
	void operator=(mat3 const& other) {
		m1 = other.m1;
		m2 = other.m2;
		m3 = other.m3;
	}
	mat3 operator*(double d) const {
		return mat3(m1 * d, m2 * d, m3 * d);
	}
	vec3 operator*(vec3 const& r) const {
		return vec3(dot(mat[0], r), dot(mat[1], r), dot(mat[2], r));
	}
	mat3 operator*(mat3 const& other)const {
		mat3 other_t = other.T();
		mat3 res;
		for (int i = 0; i < 3; i += 1)
			for (int j = 0; j < 3; j += 1)
				res.mat[i][j] = dot(this->mat[i], other_t.mat[j]);
		return res;
	}
	vec3& operator[](int idx) {
		return mat[idx];
	}
	vec3 operator[](int idx) const {
		return mat[idx];
	}
	template<class T>
	mat3& operator*=(T const& other) {
		*this = *this * other;
		return *this;
	}
	mat3 T() const {
		return mat3(vec3(m1.x, m2.x, m3.x), vec3(m1.y, m2.y, m3.y), vec3(m1.z, m2.z, m3.z));
	}
};

MAT_OP(+, +=);
MAT_OP(-, -=);

vec3 operator*(vec3 const& v, mat3 const& m) {
	return m.T() * v;
}
mat3 operator*(double d, mat3 const& m) {
	return m * d;
}

mat3 I = mat3({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 });

mat3 cross_repr(vec3 v) {
	return mat3({ 0, -v.z, v.y }, { v.z, 0, -v.x }, { -v.y, v.x, 0 });
}
mat3 rot(vec3 axis, double phi) {
	//Rodrigues' rotation formula
	mat3 w = cross_repr(axis);
	return I + w * sin(phi) + w * w * (1 - cos(phi));
}

