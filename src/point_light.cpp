#include "point_light.h"

#include <iostream>

#include "shader.h"

using namespace std;

PointLight::PointLight(const glm::vec3 &pos) : Renderable(false), pos_(pos) {}

PointLight::~PointLight() { CleanUp(); }

void PointLight::LoadData(Shader *shader) const {
  if (!shader->CopyDataToUniform(ambient_, "pointLight.ambient")) {
    cerr << "Point light ambient not loaded to shader" << endl;
  }
  if (!shader->CopyDataToUniform(diffuse_, "pointLight.diffuse")) {
    cerr << "Point light diffuse not loaded to shader" << endl;
  }
  if (!shader->CopyDataToUniform(specular_, "pointLight.specular")) {
    cerr << "Point light specular not loaded to shader" << endl;
  }
  if (!shader->CopyDataToUniform(pos_, "pointLight.position")) {
    cerr << "Point light position not loaded to shader" << endl;
  }
}

void PointLight::GenerateCubeMaps() const {}

void PointLight::SetData() {
  vertexCount_ = 1;
  vertices_ = new Vertex({pos_, glm::vec3(0, 0, 1), glm::vec3(1)});

  indexCount_ = 1;
  indices_ = new unsigned int(0);
}
