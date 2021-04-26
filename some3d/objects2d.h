#pragma once
#include "vectors.h"
#include "objects3d.h"

class Rectangle {
public:
	vec3 vertex;
	vec3 side_1_vec;
	vec3 side_2_vec;
	Rectangle(vec3 const & vertex_, vec3 const & side_1_vec_, vec3 const & side_2_vec_):
		vertex(vertex_),
		side_1_vec(side_1_vec_),
		side_2_vec(side_2_vec_){}

	vec3 intersect(vec3 const & ro, vec3 const & rd) {
		vec3 n = cross(side_1_vec, side_2_vec);
		double d = dot(vertex - ro, n) / dot(rd, n);
		if (d > 50 or d < 0) {
			return vec3(0.);
		}
		vec3 point = ro + rd * d;
		if (dot(point - vertex, side_1_vec) >= 0 and dot(point - vertex - side_1_vec, side_1_vec) <= 0 and
			dot(point - vertex, side_2_vec) >= 0 and dot(point - vertex - side_2_vec, side_2_vec) <= 0)
			return point;
		return vec3(0.);
	}
};