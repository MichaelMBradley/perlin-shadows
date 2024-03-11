#include "grid.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

#include "constants.h"
#include "noise_math.h"
#include "random.h"

using namespace std;

Grid Grid::operator+(const Grid &other) const {
  Grid result;
  for (size_t i = 0; i < data_->size(); ++i) {
    (*result.data_)[i] = (*data_)[i] + (*other.data_)[i];
  }
  return result;
}

void Grid::operator+=(const Grid &other) {
  for (size_t i = 0; i < data_->size(); ++i) {
    (*data_)[i] += (*other.data_)[i];
  }
}

Grid Grid::operator-() const { return operator*(-1.); }

Grid Grid::operator-(const Grid &other) const { return operator+(-other); }

void Grid::operator-=(const Grid &other) { operator+=(-other); }

Grid Grid::operator*(const double val) const {
  Grid result;
  for (size_t i = 0; i < data_->size(); ++i) {
    (*result.data_)[i] = (*data_)[i] * val;
  }
  return result;
}

void Grid::operator*=(const double val) {
  for (size_t i = 0; i < data_->size(); ++i) {
    (*data_)[i] *= val;
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
Grid *Grid::PerlinNoise(std::size_t detail) {
  auto grid = new Grid();

  // Stores the vectors at grid corners as just angles as they're all normalised
  const size_t major_width = (kGeographyWidth / detail) + 1;
  const size_t major_length = (kGeographyLength / detail) + 1;
  const size_t grid_nodes = major_width * major_length;
  vector<double> sin_major_angles(grid_nodes);
  vector<double> cos_major_angles(grid_nodes);

  // Randomly generates corner vector angles [0, 2pi)
  for (size_t i = 0; i < grid_nodes; ++i) {
    auto angle = UniformDouble(0., 2 * glm::pi<double>());
    sin_major_angles[i] = sin(angle);
    cos_major_angles[i] = cos(angle);
  }

  // For each point in space
  for (size_t x = 0; x < kGeographyWidth; ++x) {
    // Determines the x index of one of the grid vectors around the point
    const size_t lower_major_x = x / detail;
    // Determines the offset from the x position of the above grid vector.
    // Offsets by 0.5 to sample from the middle of the point and avoid
    // being on grid lines, but I don't think this is technically necessary
    const double x_offset =
        (.5 + static_cast<double>(x % detail)) / static_cast<double>(detail);

    for (size_t y = 0; y < kGeographyLength; ++y) {
      // Determines the y index of one of the grid vectors around the point
      const size_t lower_major_y = y / detail;

      // Same logic as x_offset
      const double y_offset =
          (.5 + static_cast<double>(y % detail)) / static_cast<double>(detail);

      // Lambda to determine the dot product of the offset vector of the current
      // point with one of the four grid vectors around the current point
      const auto dot_major = [=, &cos_major_angles, &sin_major_angles](
                                 bool high_x, bool high_y) -> double {
        const auto major_x = lower_major_x + (high_x ? 1 : 0);
        const auto major_y = lower_major_y + (high_y ? 1 : 0);
        const auto sin_major_angle =
            sin_major_angles[major_y * major_width + major_x];
        const auto cos_major_angle =
            cos_major_angles[major_y * major_width + major_x];
        const auto x_major_offset = x_offset - (high_x ? 1 : 0);
        const auto y_major_offset = y_offset - (high_y ? 1 : 0);
        return cos_major_angle * x_major_offset +
               sin_major_angle * y_major_offset;
      };

      // Interpolates between the dot_major results of the four grid vectors
      // around the current point
      const auto val = Interpolate(
          x_offset,
          Interpolate(y_offset, dot_major(false, false),
                      dot_major(false, true)),
          Interpolate(y_offset, dot_major(true, false), dot_major(true, true)));
      grid->set(x, y, val);
    }
  }

  grid->CalculateMinMax();
  return grid;
}
