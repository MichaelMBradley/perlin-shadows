#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include "shader.h"

class Camera {
 public:
  explicit Camera(const int width, const int height) {
    set_aspect(width, height);
  }

  void LoadMatrices(Shader *shader) const;

  inline void RelativeRotate(const glm::vec3 amount) { rotation_ += amount; }
  inline void RelativeMove(const glm::vec3 amount) {
    position_ += amount.x * look_vector() + amount.y * normal_vector() +
                 amount.z * up_vector();
  }
  inline void AbsoluteMove(const glm::vec3 amount) { position_ += amount; }

  inline void set_aspect(const int width, const int height) {
    aspect_ = static_cast<float>(width) / static_cast<float>(height);
  }

 private:
  inline glm::vec3 look_vector() const {
    return glm::rotateZ(glm::rotateY(glm::vec3(1., 0., 0.), rotation_.y),
                        rotation_.z);
  }
  inline glm::vec3 up_vector() const {
    return glm::rotateZ(glm::rotateY(glm::vec3(0., 0., 1.), rotation_.y),
                        rotation_.z);
  }
  inline glm::vec3 normal_vector() const {
    return glm::cross(up_vector(), look_vector());
  }

  glm::vec3 position_{0, 0, 2};
  // This vector is not physically representative of the camera's rotation,
  // rather the x component controls the roll, the y component controls the
  // pitch, and the z component controls the yaw.
  glm::vec3 rotation_{0, glm::half_pi<float>(), 0};
  float aspect_{1};
};
