#pragma once

#include <array>
#include <glm/glm.hpp>
#include <memory>

#include "constants.h"
#include "noise_math.h"
#include "shader.h"

constexpr std::size_t index(const std::size_t x, const std::size_t y) {
  return x + y * kGeographyShort;
}

class Grid {
 public:
  inline float get(const std::size_t x, const std::size_t y) const {
    return (*data_)[index(x, y)];
  }
  inline void set(const std::size_t x, const std::size_t y, float value) {
    (*data_)[index(x, y)] = value;
  }

  Grid operator+(const Grid &) const;
  void operator+=(const Grid &);
  Grid operator-() const;
  Grid operator-(const Grid &) const;
  void operator-=(const Grid &);
  Grid operator*(float) const;
  void operator*=(float);
  Grid operator/(float) const;
  void operator/=(float);

  glm::vec3 normal_at(std::size_t, std::size_t, float = 1.) const;

  static Grid *PerlinNoise(std::size_t);

  std::array<Vertex, kTotalVertices> *vertices() const;
  static std::array<unsigned int, kTotalIndices> *indices();

 private:
  std::unique_ptr<std::array<float, kGeographyShort * kGeographyLong>> data_{
      std::make_unique<std::array<float, kGeographyShort * kGeographyLong>>()};
};
