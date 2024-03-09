#include "grid.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

#include "constants.h"
#include "noise_math.h"
#include "random.h"

using namespace std;

double Grid::sample(const double x, const double y) const {
  const auto min_x = static_cast<size_t>(x);
  const auto min_y = static_cast<size_t>(y);

  if (x == static_cast<double>(min_x) && y == static_cast<double>(min_y)) {
    return get(min_x, min_y);
  }

  const glm::dvec3 min_min = glm::dvec3(static_cast<double>(min_x) - x,
                                        static_cast<double>(min_y) - y, 0.);
  const glm::dvec3 min_max = glm::dvec3(
      static_cast<double>(min_x) - x, static_cast<double>(min_y) - y + 1., 0.);
  const glm::dvec3 max_min = glm::dvec3(static_cast<double>(min_x) - x + 1.,
                                        static_cast<double>(min_y) - y, 0.);
  const glm::dvec3 max_max =
      glm::dvec3(static_cast<double>(min_x) - x + 1.,
                 static_cast<double>(min_y) - y + 1., 0.);

  double sum = 0.;
  double weights = 0.;

  double weight = 1. / glm::length(min_min);
  weights += weight;
  sum += get(min_x, min_y) * weight;

  weight = 1. / glm::length(min_max);
  weights += weight;
  sum += get(min_x, min_y + 1) * weight;

  weight = 1. / glm::length(max_min);
  weights += weight;
  sum += get(min_x + 1, min_y) * weight;

  weight = 1. / glm::length(max_max);
  weights += weight;
  sum += get(min_x + 1, min_y + 1) * weight;

  return sum / weights;
}

void Grid::put(const double x, const double y, const double value) {
  const auto min_x = static_cast<size_t>(x);
  const auto min_y = static_cast<size_t>(y);

  if (x == static_cast<double>(min_x) && y == static_cast<double>(min_y)) {
    return set(min_x, min_y, value);
  }

  const glm::dvec3 min_min = glm::dvec3(static_cast<double>(min_x) - x,
                                        static_cast<double>(min_y) - y, 0.);
  const glm::dvec3 min_max = glm::dvec3(
      static_cast<double>(min_x) - x, static_cast<double>(min_y) - y + 1., 0.);
  const glm::dvec3 max_min = glm::dvec3(static_cast<double>(min_x) - x + 1.,
                                        static_cast<double>(min_y) - y, 0.);
  const glm::dvec3 max_max =
      glm::dvec3(static_cast<double>(min_x) - x + 1.,
                 static_cast<double>(min_y) - y + 1., 0.);

  const double nnl = glm::length(min_min);
  const double nxl = glm::length(min_max);
  const double xnl = glm::length(max_min);
  const double xxl = glm::length(max_max);
  const double weights = (nnl + nxl + xnl + xxl) / (nnl * nxl * xnl * xxl);

  set(min_x, min_y, value * nnl / weights);
  set(min_x, min_y + 1, value * nxl / weights);
  set(min_x + 1, min_y, value * xnl / weights);
  set(min_x + 1, min_y + 1, value * xxl / weights);
}

Grid Grid::operator+(const Grid &other) const {
  Grid result;
  for (size_t x = 0; x < kGeographyWidth; ++x) {
    for (size_t y = 0; y < kGeographyLength; ++y) {
      result.set(x, y, get(x, y) + other.get(x, y));
    }
  }
  return result;
}

void Grid::operator+=(const Grid &other) {
  for (size_t x = 0; x < kGeographyWidth; ++x) {
    for (size_t y = 0; y < kGeographyLength; ++y) {
      set(x, y, get(x, y) + other.get(x, y));
    }
  }
}

Grid Grid::operator-() const {
  Grid result;
  for (size_t x = 0; x < kGeographyWidth; ++x) {
    for (size_t y = 0; y < kGeographyLength; ++y) {
      result.set(x, y, -get(x, y));
    }
  }
  return result;
}

Grid Grid::operator-(const Grid &other) const { return operator+(-other); }

void Grid::operator-=(const Grid &other) { operator+=(-other); }

Grid Grid::operator*(const double val) const {
  Grid result;
  for (size_t x = 0; x < kGeographyWidth; ++x) {
    for (size_t y = 0; y < kGeographyLength; ++y) {
      result.set(x, y, get(x, y) * val);
    }
  }
  return result;
}

void Grid::operator*=(const double val) {
  for (size_t x = 0; x < kGeographyWidth; ++x) {
    for (size_t y = 0; y < kGeographyLength; ++y) {
      set(x, y, get(x, y) * val);
    }
  }
}

Grid Grid::operator/(const double val) const { return operator*(1 / val); }

void Grid::operator/=(const double val) { operator*=(1 / val); }

glm::dvec3 Grid::normal_at(const size_t x, const size_t y,
                           const double amplification) const {
  const auto low_x = x == 0 ? 0 : x - 1;
  const auto high_x = x == kGeographyWidth - 1 ? x : x + 1;
  const auto low_y = y == 0 ? 0 : y - 1;
  const auto high_y = y == kGeographyLength - 1 ? y : y + 1;

  const auto x_diff =
      glm::dvec3(static_cast<double>(high_x - low_x), 0.,
                 amplification * (get(high_x, y) - get(low_x, y)));
  const auto y_diff =
      glm::dvec3(0., static_cast<double>(high_y - low_y),
                 amplification * (get(x, high_y) - get(x, low_y)));
  return glm::normalize(glm::cross(x_diff, y_diff));
}

// Implementation based on: https://en.wikipedia.org/wiki/Perlin_noise
// Assumes that kGeographyWidth and kGeographyLength are equal to 2^n (doesn't
// have to be the same n)
void Grid::PerlinNoise(std::size_t detail, double amplitude) {
  // Stores the vectors at grid corners as just angles as they're all normalised
  const size_t major_width = (kGeographyWidth / detail) + 1;
  const size_t major_length = (kGeographyLength / detail) + 1;
  const size_t grid_nodes = major_width * major_length;
  vector<double> major_angles(grid_nodes);

  // Randomly generates corner vector angles [0, 2pi)
  for (size_t i = 0; i < grid_nodes; ++i) {
    major_angles[i] = UniformDouble(0., 2 * glm::pi<double>());
  }

  // For each point in space
  for (size_t x = 0; x < kGeographyWidth; ++x) {
    for (size_t y = 0; y < kGeographyLength; ++y) {
      // Determines the x and y indices of one of the grid vectors around the
      // point
      const size_t lower_major_x = x / detail;
      const size_t lower_major_y = y / detail;

      // Determines the offset from the x and y positions of the above grid
      // vector Offsets by 0.5 to sample from the middle of the point and avoid
      // being on grid lines, but I don't think this is technically necessary
      const double x_offset =
          (.5 + static_cast<double>(x % detail)) / static_cast<double>(detail);
      const double y_offset =
          (.5 + static_cast<double>(y % detail)) / static_cast<double>(detail);

      // Lambda to determine the dot product of the offset vector of the current
      // point with one of the four grid vectors around the current point
      const auto dot_major = [=](bool high_x, bool high_y) -> double {
        const auto major_x = lower_major_x + (high_x ? 1 : 0);
        const auto major_y = lower_major_y + (high_y ? 1 : 0);
        const auto major_angle = major_angles[major_y * major_width + major_x];
        const auto x_major_offset = x_offset - (high_x ? 1 : 0);
        const auto y_major_offset = y_offset - (high_y ? 1 : 0);
        return cos(major_angle) * x_major_offset +
               sin(major_angle) * y_major_offset;
      };

      // Interpolates between the dot_major results of the four grid vectors
      // around the current point
      const auto val = Interpolate(
          x_offset,
          Interpolate(y_offset, dot_major(false, false),
                      dot_major(false, true)),
          Interpolate(y_offset, dot_major(true, false), dot_major(true, true)));
      set(x, y, get(x, y) + val * amplitude);
    }
  }

  CalculateMinMax();
}
