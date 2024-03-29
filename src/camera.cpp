#include "camera.h"

#include <glm/ext/matrix_transform.hpp>

void Camera::LoadMatrices(Shader *shader) const {
  const auto viewMatrix =
      glm::lookAt(position_, position_ + look_vector(), up_vector());
  auto success = shader->CopyDataToUniform(viewMatrix, "view");
  assert(success);

  const auto perspectiveMatrix =
      glm::perspective(glm::radians(45.), aspect_, .1, 100.);
  success = shader->CopyDataToUniform(perspectiveMatrix, "projection");
  assert(success);
}
