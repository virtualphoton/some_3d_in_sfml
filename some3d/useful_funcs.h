#pragma once

inline double clamp(double val, double min, double max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}