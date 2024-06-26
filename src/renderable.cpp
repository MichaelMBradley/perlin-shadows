#include "renderable.h"

#include <iostream>

using namespace std;

Renderable::Renderable(bool drawTriangles) : drawTriangles_(drawTriangles) {}

Renderable::~Renderable() { CleanUp(); }

void Renderable::InitGeom() {
  SetData();

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<long>(sizeof(Vertex)) * vertexCount_, vertices_,
               GL_STATIC_DRAW);
  delete[] vertices_;
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<long>(sizeof(unsigned int)) * indexCount_, indices_,
               GL_STATIC_DRAW);
  delete[] indices_;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderable::Render(const Shader *const shader) const {
  if (!shader->CopyDataToUniform(model_, "model")) {
    // cerr << "Model matrix not in shader" << endl;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

  GLuint attribLoc = glGetAttribLocation(shader->id(), "vtxPos");
  if (attribLoc != -1) {
    glEnableVertexAttribArray(attribLoc);
    glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, position));
  } else {
    // cerr << "Not loading vtxPos" << endl;
  }

  attribLoc = glGetAttribLocation(shader->id(), "vtxNormal");
  if (attribLoc != -1) {
    glEnableVertexAttribArray(attribLoc);
    glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
  } else {
    // cerr << "Not loading vtxNormal" << endl;
  }

  if (drawTriangles_) {
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
  } else {
    glPointSize(9);
    glDrawArrays(GL_POINTS, 0, vertexCount_);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderable::CleanUp() {
  if (vbo_ == 0 && ebo_ == 0) {
    return;
  }
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  vbo_ = 0;
  ebo_ = 0;
}
