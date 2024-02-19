#include "grid.h"

using namespace std;


template<size_t w, size_t l>
Grid<w, l> Grid<w, l>::operator+(const Grid<w, l> &other) const {
	Grid<w, l> result;
	for (size_t x = 0; x < w; ++x) {
		for (size_t y = 0; y < l; ++y) {
			result.set(x, y, get(x, y) + other.get(x, y));
		}
	}
	return result;
}

template<size_t w, size_t l>
void Grid<w, l>::operator+=(const Grid<w, l> &other) {
	for (size_t x = 0; x < w; ++x) {
		for (size_t y = 0; y < l; ++y) {
			set(x, y, get(x, y) + other.get(x, y));
		}
	}
}

template<size_t w, size_t l>
Grid<w, l> Grid<w, l>::operator-() const {
	Grid<w, l> result;
	for (size_t x = 0; x < w; ++x) {
		for (size_t y = 0; y < l; ++y) {
			result.set(x, y, -get(x, y));
		}
	}
	return result;
}

template<size_t w, size_t l>
Grid<w, l> Grid<w, l>::operator-(const Grid<w, l> &other) const {
	return operator+(-other);
}

template<size_t w, size_t l>
void Grid<w, l>::operator-=(const Grid<w, l> &other) {
	operator+=(-other);
}

template<size_t w, size_t l>
Grid<w, l> Grid<w, l>::operator*(const double val) const {
	Grid<w, l> result;
	for (size_t x = 0; x < w; ++x) {
		for (size_t y = 0; y < l; ++y) {
			result.set(x, y, get(x, y) * val);
		}
	}
	return result;
}

template<size_t w, size_t l>
void Grid<w, l>::operator*=(const double val) {
	for (size_t x = 0; x < w; ++x) {
		for (size_t y = 0; y < l; ++y) {
			set(x, y, get(x, y) * val);
		}
	}
}

template<size_t w, size_t l>
Grid<w, l> Grid<w, l>::operator/(const double val) const {
	return operator*(1 / val);
}

template<size_t w, size_t l>
void Grid<w, l>::operator/=(const double val) {
	operator*=(1 / val);
}

template<size_t w, size_t l>
Grid<w, l> Grid<w, l>::perlin_noise(const size_t detail) {
	perlin_noise(time(0), detail);
}

// Implementation based on: https://en.wikipedia.org/wiki/Perlin_noise
// Assumes that width and length are equal to 2^n (doesn't have to be the same n)
template<size_t w, size_t l>
Grid<w, l> Grid<w, l>::perlin_noise(const int seed, const size_t detail) {
	// Initializes a random number generator
	random_device device;
	default_random_engine engine(device());
	engine.seed(seed);
	uniform_real_distribution<double> uniform(0, 2. * M_PI);

	Grid<w, l> data;

	// Stores the vectors at grid corners as just angles, as they're all normalised anyways
	constexpr size_t major_width = (w / detail) + 1;
	constexpr size_t major_length = (l / detail) + 1;
	constexpr size_t grid_nodes = major_width * major_length;
	array<double, grid_nodes> major_angles = {};

	// Randomly generates corner vector angles [0, 2pi)
	for (size_t i = 0; i < grid_nodes; ++i) {
		major_angles[i] = uniform(engine);
	}

	// For each point in space
	for (size_t x = 0; x < w; ++x) {
		for (size_t y = 0; y < l; ++y) {
			// Determines the x and y indices of one of the grid vectors around the point
			const size_t lower_major_x = x / detail;
			const size_t lower_major_y = y / detail;

			// Determines the offset from the x and y positions of the above grid vector
			// Offsets by 0.5 to sample from the middle of the point and avoid being on grid lines,
			// but I don't think this is technically necessary
			const double x_offset = (.5 + static_cast<double>(x % detail)) / detail;
			const double y_offset = (.5 + static_cast<double>(y % detail)) / detail;

			// Lambda to determine the dot product of the offset vector of the current point with
			// one of the four grid vectors around the current point
			const auto dot_major = [=] (bool high_x, bool high_y) {
				const auto major_x = lower_major_x + (high_x ? 1 : 0);
				const auto major_y = lower_major_y + (high_y ? 1 : 0);
				const auto major_angle = major_angles[major_y * major_width + major_x];
				const auto x_major_offset = x_offset - (high_x ? 1 : 0);
				const auto y_major_offset = y_offset - (high_y ? 1 : 0);
				return cos(major_angle) * x_major_offset + sin(major_angle) * y_major_offset;
			};

			// Interpolates between the dot_major results of the four grid vectors around the current point
			const auto val = interpolate(
				x_offset,
				interpolate(
					y_offset,
					dot_major(false, false),
					dot_major(false, true)
				),
				interpolate(
					y_offset,
					dot_major(true, false),
					dot_major(true, true)
				)
			);
			//set(x, y, (val + 1) * 10);
			data.set(x, y, val);
		}
	}

	return data;
}
