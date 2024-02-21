#pragma once

#include <array>

#include "constants.h"
#include "math.h"
#include "vec.h"

class Grid {
public:
	inline double get (const std::size_t x, const std::size_t y) const {
		return data[x + y * width];
	}
	inline void set (const std::size_t x, const std::size_t y, double value) {
		data[x + y * width] = value;
		if (value < minimum) { minimum = value; }
		if (value > maximum) { maximum = value; }
	}

	Grid operator+(const Grid&) const;
	void operator+=(const Grid&);
	Grid operator-() const;
	Grid operator-(const Grid&) const;
	void operator-=(const Grid&);
	Grid operator*(const double) const;
	void operator*=(const double);
	Grid operator/(const double) const;
	void operator/=(const double);

	inline double min() const { return minimum; }
	inline double max() const { return maximum; }

	Vec3 normal_at(const std::size_t, const std::size_t, const double = 1.) const;

	void perlin_noise(const std::size_t, const double);

private:
	double minimum { 0. };
	double maximum { 0. };

	std::array<double, width * length> data {};
};

