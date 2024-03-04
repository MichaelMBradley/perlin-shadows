#pragma once

// Based on Ken Perlin's smoother step function: https://en.wikipedia.org/wiki/Smoothstep#Variations
// Returns value in range [0, 1]
constexpr double smoother_step(const double x) {
	if (x < 0) { return 0; }
	if (x > 1) { return 1; }
	// f(0) = 0, f(1) = 1, and first and second derivatives 0 and x=0 and x=1
	return x * x * x * (x * (6. * x - 15.) + 10.);
}

// Interpolates between two doubles
// If x <= 0 returns bound_0, if x >= 1 returns bound_1,
// otherwise smoothly transitions between the two
constexpr double interpolate(const double x, const double bound_0, const double bound_1) {
	return bound_0 + smoother_step(x) * (bound_1 - bound_0);
}
