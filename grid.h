#pragma once

#include <array>

#include "constants.h"
#include "math.h"

class Grid {
public:
	inline double get (const std::size_t x, const std::size_t y) const { return data[x + y * width]; }
	inline void set (const std::size_t x, const std::size_t y, double value) { data[x + y * width] = value; }

	Grid operator+(const Grid&) const;
	void operator+=(const Grid&);
	Grid operator-() const;
	Grid operator-(const Grid&) const;
	void operator-=(const Grid&);
	Grid operator*(const double) const;
	void operator*=(const double);
	Grid operator/(const double) const;
	void operator/=(const double);

	void perlin_noise(const std::size_t, const double);
	void perlin_noise(const int, const std::size_t, const double);

private:
	std::array<double, width * length> data {};
};

