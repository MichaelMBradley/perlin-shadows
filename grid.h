#pragma once

#include <array>
#include <cmath>
#include <ctime>
#include <random>

#include "math.h"

template<std::size_t w, std::size_t l>
class Grid {
public:
	inline double get (const std::size_t x, const std::size_t y) const { return data[x + y * w]; }
	inline void set (const std::size_t x, const std::size_t y, double value) { data[x + y * w] = value; }

	Grid<w, l> operator+(const Grid<w, l>&) const;
	void operator+=(const Grid<w, l>&);
	Grid<w, l> operator-() const;
	Grid<w, l> operator-(const Grid<w, l>&) const;
	void operator-=(const Grid<w, l>&);
	Grid<w, l> operator*(const double) const;
	void operator*=(const double);
	Grid<w, l> operator/(const double) const;
	void operator/=(const double);

	static Grid<w, l> perlin_noise(const std::size_t);
	static Grid<w, l> perlin_noise(const int, const std::size_t);

private:
	std::array<double, w * l> data {};
};

