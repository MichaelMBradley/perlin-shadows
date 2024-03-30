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
      glm::perspective(glm::radians(45.0f), aspect_, 0.1f,
                       static_cast<float>(kGeographyLong * 3));
  if (!shader->CopyDataToUniform(perspectiveMatrix, "projection")) {
    cerr << "Projection matrix not in shader" << endl;
  }

  if (!shader->CopyDataToUniform(position_, "camera")) {
    cerr << "Camera position not in shader" << endl;
  }
}
