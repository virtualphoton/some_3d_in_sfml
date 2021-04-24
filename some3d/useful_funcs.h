#pragma once
#include <string>
#include <vector>

using std::string;

double clamp(double val, double min, double max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

template <class Vtype, class Head>
void vec_from_var_args(std::vector<Vtype> & vec, Head & s) {
	vec.push_back(s);
}

template <class Vtype, class Head, class ... Tail>
void vec_from_var_args(std::vector<Vtype> & vec, Head s, Tail ... rep1) {
	vec.push_back(s);
	vec_from_var_args(vec, rep1...);
}

template <class ...Tail>
string format(string const & s, Tail const &... rep_tail) {
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
		} else if (s[i] == '}') {
			sub_len = 0;
			sub_start = i + 1;
		} else
			sub_len += 1;
	}
	out.append(s.substr(sub_start, sub_len));
	return out;
}
