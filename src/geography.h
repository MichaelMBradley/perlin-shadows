#pragma once

#include <glm/glm.hpp>

#include "grid.h"

class Geography {
 public:
  Geography();

  void randomize();

  inline double height_at(const std::size_t x, const std::size_t y) const {
    return height.get(x, y);
  }
  inline glm::dvec3 normal_at(const std::size_t x, const std::size_t y,
                              const double amplification = 1.) const {
    return height.normal_at(x, y, amplification);
  }

  inline double min_height() const { return height.min(); }
  inline double max_height() const { return height.max(); }

 private:
  Grid height;
};
