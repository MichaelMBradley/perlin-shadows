#include "grid.h"

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "constants.h"
#include "noise_math.h"

using namespace std;

random_device Grid::device_;
default_random_engine::result_type Grid::base_random_{0};

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

Grid Grid::operator-() const { return operator*(-1); }

Grid Grid::operator-(const Grid &other) const { return operator+(-other); }

void Grid::operator-=(const Grid &other) { operator+=(-other); }

Grid Grid::operator*(const float val) const {
  Grid result;
  for (size_t i = 0; i < data_->size(); ++i) {
    (*result.data_)[i] = (*data_)[i] * val;
  }
  return result;
}

void Grid::operator*=(const float val) {
  for (size_t i = 0; i < data_->size(); ++i) {
    (*data_)[i] *= val;
  }
}

Grid Grid::operator/(const float val) const { return operator*(1 / val); }

void Grid::operator/=(const float val) { operator*=(1 / val); }

glm::vec3 Grid::normal_at(const size_t x, const size_t y,
                          const float amplification) const {
  const auto low_x = x == 0 ? 0 : x - 1;
  const auto high_x = x == kGeographyShort - 1 ? x : x + 1;
  const auto low_y = y == 0 ? 0 : y - 1;
  const auto high_y = y == kGeographyLong - 1 ? y : y + 1;

  const auto x_diff =
      glm::vec3(static_cast<float>(high_x - low_x), 0,
                amplification * (get(high_x, y) - get(low_x, y)));
  const auto y_diff =
      glm::vec3(0, static_cast<float>(high_y - low_y),
                amplification * (get(x, high_y) - get(x, low_y)));
  return glm::normalize(glm::cross(x_diff, y_diff));
}

// Implementation based on: https://en.wikipedia.org/wiki/Perlin_noise
// Assumes that kGeographyShort and kGeographyLong are equal to 2^n (doesn't
// have to be the same n)
Grid *Grid::PerlinNoise(int globalX, int globalY, std::size_t detail) {
  auto grid = new Grid();

  // Store the vectors at grid corners as just angles, they're all normalised
  const size_t major_width = (kGeographyShort / detail) + 1;
  const size_t major_length = (kGeographyLong / detail) + 1;
  const size_t grid_nodes = major_width * major_length;
  vector<float> sin_major_angles(grid_nodes);
  vector<float> cos_major_angles(grid_nodes);

  auto worldIndexX = (major_width - 1) * globalX;
  auto worldIndexY = ((major_width - 1) * kGeographyCountShort + 1) *
                     (major_length - 1) * globalY;
  auto worldIndex = worldIndexX + worldIndexY;
  mt19937::result_type gridBaseSeed = base_random_ * detail + worldIndex;

  // Randomly generates corner vector angles [0, 2pi)
  for (size_t x = 0; x < major_width; ++x) {
    for (size_t y = 0; y < major_width; ++y) {
      auto relativeWorldIndex =
          x + ((major_width - 1) * kGeographyCountShort + 1) * y;
      grid->AngleSeed(gridBaseSeed + relativeWorldIndex);
      auto angle = grid->RandomAngle();
      auto i = x + major_width * y;
      sin_major_angles[i] = sin(angle);
      cos_major_angles[i] = cos(angle);
    }
  }

  // For each point in space
  for (size_t x = 0; x < kGeographyShort; ++x) {
    // Determines the x index of one of the grid vectors around the point
    const size_t lower_major_x = x / detail;
    // Determines the offset from the x position of the above grid vector.
    // Offsets by 0.5 to sample from the middle of the point and avoid
    // being on grid lines, but I don't think this is technically necessary
    const float x_offset =
        (.5f + static_cast<float>(x % detail)) / static_cast<float>(detail);

    for (size_t y = 0; y < kGeographyLong; ++y) {
      // Determines the y index of one of the grid vectors around the point
      const size_t lower_major_y = y / detail;

      // Same logic as x_offset
      const float y_offset =
          (.5f + static_cast<float>(y % detail)) / static_cast<float>(detail);

      // Lambda to determine the dot product of the offset vector of the current
      // point with one of the four grid vectors around the current point
      const auto dot_major = [=, &cos_major_angles, &sin_major_angles](
                                 bool high_x, bool high_y) -> float {
        const auto major_x = lower_major_x + (high_x ? 1 : 0);
        const auto major_y = lower_major_y + (high_y ? 1 : 0);
        const auto sin_major_angle =
            sin_major_angles[major_y * major_width + major_x];
        const auto cos_major_angle =
            cos_major_angles[major_y * major_width + major_x];
        const auto x_major_offset = x_offset - (high_x ? 1.0f : 0.0f);
        const auto y_major_offset = y_offset - (high_y ? 1.0f : 0.0f);
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

  return grid;
}

array<Vertex, kTotalVertices> *Grid::vertices() const {
  auto vertices = new array<Vertex, kTotalVertices>();
  for (size_t x = 0; x < kGeographyShort; ++x) {
    for (size_t y = 0; y < kGeographyLong; ++y) {
      auto ind = index(x, y);
      glm::vec3 position = {x, y, (*data_)[ind]};
      glm::vec3 normal = normal_at(x, y);
      (*vertices)[ind] = {position, normal};
    }
  }
  return vertices;
}

array<unsigned int, kTotalIndices> *Grid::indices() {
  auto indices = new array<unsigned int, kTotalIndices>();
  for (size_t x = 0; x < kGeographyShort - 1; ++x) {
    for (size_t y = 0; y < kGeographyLong - 1; ++y) {
      auto indexInd = (x + y * (kGeographyShort - 1)) * 6;
      (*indices)[indexInd] = static_cast<int>(index(x, y));
      (*indices)[indexInd + 1] = static_cast<int>(index(x, y + 1));
      (*indices)[indexInd + 2] = static_cast<int>(index(x + 1, y));
      (*indices)[indexInd + 3] = static_cast<int>(index(x + 1, y));
      (*indices)[indexInd + 4] = static_cast<int>(index(x, y + 1));
      (*indices)[indexInd + 5] = static_cast<int>(index(x + 1, y + 1));
    }
  }
  return indices;
}
