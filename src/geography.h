#pragma once

#include "grid.h"

class Geography {
public:
	Geography();

    void randomize();

	inline double height_at(const std::size_t x, const std::size_t y) const { return height.get(x, y); }
	inline Vec3 normal_at(const std::size_t x, const std::size_t y, const double amplification = 1.) const { return height.normal_at(x, y, amplification); }

	inline double min_height() const { return height.min(); }
	inline double max_height() const { return height.max(); }

	void simulate_drop(double, double);
	void simulate_random_drop();

private:
	Grid height;
};
