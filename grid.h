#pragma once

#include <array>

template<typename T, std::size_t w, std::size_t l>
class Grid {
public:
	inline const T get (const std::size_t x, const std::size_t y) const { return data[x + y * w]; }
	inline void set (const std::size_t x, const std::size_t y, T value) { data[x + y * w] = value; }

private:
	std::array<T, w * l> data = {};
};

