#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
};

class Shader {
 public:
  Shader(const std::string &, const std::string &);
  ~Shader();

  bool CopyDataToUniform(const glm::dmat4 &, const std::string &) const;
  bool CopyDataToUniform(const glm::dvec3 &, const std::string &) const;
  bool CopyDataToUniform(const glm::dvec4 &, const std::string &) const;

  void PrintStatus() const;

  inline GLuint id() const { return id_; }

 private:
  GLuint id_;

  static std::string ReadFile(const std::string &);
  static GLuint CompileShader(const std::string &, GLenum);

  static void CheckGLError(const std::string &);
  static void CheckProgramivError(GLuint, GLenum, const std::string &);
  static void CheckShaderivError(GLuint, GLenum, const std::string &);
};
