#pragma once

#include "grid.h"

class Geography {
public:
	Geography();

	inline double height_at(const std::size_t x, const std::size_t y) const { return height.get(x, y); }

	void simulate_drop(size_t, size_t);
	void simulate_random_drop();

private:
	Grid height;
};

