#pragma once

#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <memory>

#include "constants.h"
#include "noise_math.h"

class Grid {
 public:
  inline double get(const std::size_t x, const std::size_t y) const {
    return (*data_)[x + y * kGeographyWidth];
  }
  inline void set(const std::size_t x, const std::size_t y, double value) {
    (*data_)[x + y * kGeographyWidth] = value;
  }

  Grid operator+(const Grid &) const;
  void operator+=(const Grid &);
  Grid operator-() const;
  Grid operator-(const Grid &) const;
  void operator-=(const Grid &);
  Grid operator*(double) const;
  void operator*=(double);
  Grid operator/(double) const;
  void operator/=(double);

  inline double min() const { return minimum_; }
  inline double max() const { return maximum_; }

  glm::dvec3 normal_at(std::size_t, std::size_t, double = 1.) const;

  static Grid *PerlinNoise(std::size_t);

 private:
  inline void CalculateMinMax() {
    const auto min_max = std::minmax_element(data_->begin(), data_->end());
    minimum_ = *min_max.first;
    maximum_ = *min_max.second;
  }

  double minimum_{0.};
  double maximum_{0.};

  std::unique_ptr<std::array<double, kGeographyWidth * kGeographyLength>> data_{
      std::make_unique<
          std::array<double, kGeographyWidth * kGeographyLength>>()};
};
