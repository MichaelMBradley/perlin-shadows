#pragma once

#include <algorithm>
#include <array>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <random>

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

  static Grid *PerlinNoise(int, int, std::size_t);

  std::array<Vertex, kTotalVertices> *vertices() const;
  static std::array<unsigned int, kTotalIndices> *indices();
  static inline void RandomizeBase() { base_random_ = device_() << 4; }

  inline float min() const {
    return *std::min_element(data_->begin(), data_->end());
  }
  inline float max() const {
    return *std::max_element(data_->begin(), data_->end());
  }

 private:
  std::unique_ptr<std::array<float, kGeographyShort * kGeographyLong>> data_{
      std::make_unique<std::array<float, kGeographyShort * kGeographyLong>>()};

  static std::random_device device_;
  std::mt19937 engine_{device_()};
  std::uniform_real_distribution<float> dist_{0, glm::two_pi<float>()};
  static std::mt19937::result_type base_random_;

  inline float RandomAngle() { return dist_(engine_); }
  inline void AngleSeed(std::mt19937::result_type seed) { engine_.seed(seed); }
};
