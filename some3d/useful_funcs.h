#pragma once
#include <string>
#include <vector>

using std::string;

// if number is in [a; b], then returns number, otherwise returns a or b, depending on which one is closer to number
double clamp(double val, double min, double max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

// func to turn variadic args into vector. Needed for format()
template <class Vtype, class Head>
void vec_from_var_args(std::vector<Vtype>& vec, Head const & s) {
	vec.push_back(s);
}
template <class Vtype, class Head, class ... Tail>
void vec_from_var_args(std::vector<Vtype>& vec, Head s, Tail const & ... rep1) {
	vec.push_back(s);
	vec_from_var_args(vec, rep1...);
}

// "unicorn-formatting", replaces all {} in string with parameters passed after it
template <class ...Tail>
string format(string const& s, Tail const &... rep_tail) {
	std::vector<string> reps;
	vec_from_var_args(reps, rep_tail...);
	string out;
	int sub_start = 0;
	int sub_len = 0;
	int reps_idx = 0;
	for (int i = 0; i < s.size(); i += 1) {
		if (s[i] == '{') {
			out.append(s.substr(sub_start, sub_len)).append(reps[reps_idx]);
			reps_idx += 1;
		}
		else if (s[i] == '}') {
			sub_len = 0;
			sub_start = i + 1;
		}
		else
			sub_len += 1;
	}
	out.append(s.substr(sub_start, sub_len));
	return out;
}

// scales and translates point from [a1; b1] to [a2; b2]
double clench(double val, double min, double max, double new_min, double new_max) {
	return clamp((val - min) / (max - min) * (new_max - new_min) + new_min, new_min, new_max);
}


#define min(a, b) __min(a, b)
#define max(a, b) __max(a, b)
//just sign function
template <class T>
int sign(T x) {
	if (x > 0)
		return 1;
	if (x < 0)
		return -1;
	return 0;
}


// just ordinary print() in ~ python style
template <class T>
void print(T const & out) {
	std::cout << out << '\n';
}
template <class Head, class ...Tail>
void print(Head const& head, Tail const &... tail) {
	std::cout << head << ' ';
	print(tail...);
}
