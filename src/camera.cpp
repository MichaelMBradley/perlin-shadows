#include "camera.h"

#include <GL/freeglut_std.h>

#include <glm/ext/matrix_transform.hpp>

void Camera::LoadMatrices() const {
  glMatrixMode(GL_MODELVIEW);
  const auto viewMatrix =
      glm::lookAt(position_, position_ + look_vector(), up_vector());
  glLoadMatrixd(&viewMatrix[0][0]);

  glMatrixMode(GL_PROJECTION);
  const auto perspectiveMatrix =
      glm::perspective(glm::radians(45.), aspect_, .1, 100.);
  glLoadMatrixd(&perspectiveMatrix[0][0]);
}
