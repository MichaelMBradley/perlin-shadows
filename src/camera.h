#pragma once

#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

class Camera {
 public:
  explicit Camera(const int width, const int height) {
    set_aspect(width, height);
  }

  void LoadMatrices() const;

  inline void RelativeRotate(const glm::dvec3 amount) { rotation_ += amount; }
  inline void RelativeMove(const glm::dvec3 amount) {
    position_ += amount.x * look_vector() + amount.y * normal_vector() +
                 amount.z * up_vector();
  }
  inline void AbsoluteMove(const glm::dvec3 amount) { position_ += amount; }

  inline void set_aspect(const int width, const int height) {
    aspect_ = static_cast<double>(width) / height;
  }

 private:
  inline glm::dvec3 look_vector() const {
    return glm::rotateZ(glm::rotateY(glm::dvec3(1., 0., 0.), rotation_.y),
                        rotation_.z);
  }
  inline glm::dvec3 up_vector() const {
    return glm::rotateZ(glm::rotateY(glm::dvec3(0., 0., 1.), rotation_.y),
                        rotation_.z);
  }
  inline glm::dvec3 normal_vector() const {
    return glm::cross(up_vector(), look_vector());
  }

  glm::dvec3 position_{0., 0., -2.};
  // This vector is not physically representative of the camera's rotation,
  // rather the x component controls the roll, the y component controls the
  // pitch, and the z component controls the yaw.
  glm::dvec3 rotation_{0., -M_PI_2, 0.};
  double aspect_{1.};
};
