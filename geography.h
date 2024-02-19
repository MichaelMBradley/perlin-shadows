#pragma once

#include <algorithm>

#include "grid.h"

class Geography {
public:
	Geography();

	inline double height_at(const std::size_t x, const std::size_t y) const { return height.get(x, y); }

	inline static constexpr std::size_t get_width() { return width; }
	inline static constexpr std::size_t get_length() { return length; }

	void simulate_drop(size_t, size_t);
	void simulate_random_drop();

private:
	static constexpr std::size_t width { 1 << 10 };
	static constexpr std::size_t length { 1 << 10 };

	Grid<width, length> height;
};

