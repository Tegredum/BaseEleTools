// encoding: utf-8
// author: Tegredum
// ide: Microsoft Visual Studio 2022
#pragma once

namespace Teg {
	template<int a, int b>
	struct min_int {
		static const int val = a < b? a : b;
	};
	template<int a, int b, int c>
	struct min_int3 {
		static const int val = min_int<min_int<a, b>::val, c>::val;
	};
	template<typename T>
	T min_teg(const T& a, const T& b) {
		return a < b? a : b;
	}
	template<int a, int b>
	struct max_int {
		static const int val = a > b? a : b;
	};
}