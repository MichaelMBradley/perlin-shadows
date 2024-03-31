#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <iostream>

using namespace std;

void Camera::LoadMatrices(Shader *shader) const {
  const auto viewMatrix =
      glm::lookAt(position_, position_ + look_vector(), up_vector());
  if (!shader->CopyDataToUniform(viewMatrix, "view")) {
    cerr << "View matrix not in shader" << endl;
  }

  const auto perspectiveMatrix =
      glm::perspective(glm::radians(kFOV), aspect_, kNearPlane, kFarPlane);
  if (!shader->CopyDataToUniform(perspectiveMatrix, "projection")) {
    cerr << "Projection matrix not in shader" << endl;
  }

  if (!shader->CopyDataToUniform(position_, "camera")) {
    cerr << "Camera position not in shader" << endl;
  }

  shader->CopyDataToUniform(kFarPlane, "farPlane");
}
