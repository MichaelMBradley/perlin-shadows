#include <cmath>
#include <ctime>
#include <random>

#include "geography.h"

using namespace std;

Geography::Geography() {
	perlin_noise();
}

// Based on Ken Perlin's smootherstep function: https://en.wikipedia.org/wiki/Smoothstep#Variations
// Returns value in range [0, 1]
constexpr double smoother_step(const double x) {
	if (x < 0) { return 0; }
	if (x > 1) { return 1; }
	// f(0) = 0, f(1) = 1, and first and second derivatives 0 and x=0 and x=1
	return x * x * x * (x * (6. * x - 15.) + 10.);
}

// Interpolates between two doubles
// If x <=0 returns bound_0, if x >= 1 returns bound_1,
// otherwise smoothly transitions between the two
constexpr double interpolate(const double x, const double bound_0, const double bound_1) {
	return bound_0 + smoother_step(x) * (bound_1 - bound_0);
}

void Geography::perlin_noise() {
	perlin_noise(time(0));
}

// Implementation based on: https://en.wikipedia.org/wiki/Perlin_noise
void Geography::perlin_noise(int seed) {
	// Initializes a random number generator
	random_device device;
	default_random_engine engine(device());
	engine.seed(seed);
	uniform_real_distribution<double> uniform(0, 2. * M_PI);

	// Stores the vectors at grid corners as just angles, as they're all normalised anyways
	constexpr size_t grid_nodes = major_width * major_length;
	array<double, grid_nodes> major_angles = {};

	// Randomly generates corner vector angles [0, 2pi)
	for (size_t i = 0; i < grid_nodes; ++i) {
		major_angles[i] = uniform(engine);
	}

	// For each point in space
	for (size_t x = 0; x < width; ++x) {
		for (size_t y = 0; y < length; ++y) {
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
			height[y * width + x] = interpolate(
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
		}
	}
}

