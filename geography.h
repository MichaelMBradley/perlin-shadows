#pragma once

#include "grid.h"

class Geography {
public:
	Geography();

	inline const double height_at(const std::size_t x, const std::size_t y) const { return height.get(x, y); }

	inline static constexpr std::size_t get_width() { return width; }
	inline static constexpr std::size_t get_length() { return length; }

private:
	void perlin_noise();
	void perlin_noise(int seed);

	// Major dimensions used during Perlin noise calculation
	static constexpr std::size_t major_width { 20 };
	static constexpr std::size_t major_length { 20 };
	static constexpr std::size_t detail { 50 };

	static constexpr std::size_t width { (major_width - 1) * detail };
	static constexpr std::size_t length { (major_length - 1) * detail };

	Grid<double, width, length> height;
};

