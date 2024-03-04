#pragma once

#include <array>

#include "constants.h"
#include "noise_math.h"
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

	double sample(double, double) const;
	void put(double, double, double);

	Grid operator+(const Grid&) const;
	void operator+=(const Grid&);
	Grid operator-() const;
	Grid operator-(const Grid&) const;
	void operator-=(const Grid&);
	Grid operator*(double) const;
	void operator*=(double);
	Grid operator/(double) const;
	void operator/=(double);

	inline double min() const { return minimum; }
	inline double max() const { return maximum; }

	Vec3 normal_at(std::size_t, std::size_t, double = 1.) const;

	void perlin_noise(std::size_t, double);

private:
	double minimum { 0. };
	double maximum { 0. };

	std::array<double, width * length> data {};
};
