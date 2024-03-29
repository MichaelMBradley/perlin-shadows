#include "shader.h"

#include <fstream>
#include <iostream>

#include "renderer.h"

using namespace std;

Shader::Shader(const string &vertexShaderFile,
               const string &fragmentShaderFile) {
  auto vertexShaderId = CompileShader(vertexShaderFile, GL_VERTEX_SHADER);
  auto fragmentShaderId = CompileShader(fragmentShaderFile, GL_FRAGMENT_SHADER);

  id_ = glCreateProgram();

  glAttachShader(id_, vertexShaderId);
  CheckGLError("Error in attaching the vertex shader");
  glAttachShader(id_, fragmentShaderId);
  CheckGLError("Error in attaching the fragment shader");

  glLinkProgram(id_);
  CheckProgramivError(id_, GL_LINK_STATUS,
                      "Error when creating shader program");

  glUseProgram(id_);
  PrintStatus();
}

// TODO: Release OpenGL data?
Shader::~Shader() = default;

const bool Shader::CopyDataToShader(const glm::mat4 &, const string &) {}

const bool Shader::CopyDataToShader(const glm::vec3 &, const string &) {}

const bool Shader::CopyDataToShader(const glm::vec4 &, const string &) {}

string Shader::ReadFile(const string &filename) {
  string data, line;
  ifstream file;
  file.open(filename);
  if (file.is_open()) {
    while (getline(file, line)) {
      data += line;
    }
  } else {
    throw runtime_error("Could not open " + filename);
  }
  return data;
}

GLuint Shader::CompileShader(const std::string &filename,
                             const GLenum shader_type) {
  const GLuint id = glCreateShader(shader_type);
  auto shaderString = ReadFile(filename);
  auto shaderC_str = shaderString.c_str();
  glShaderSource(id, 1, &shaderC_str, nullptr);
  glCompileShader(id);
  CheckShaderivError(id, GL_COMPILE_STATUS,
                     "Error when creating shader file " + filename);
  return id;
}

void Shader::PrintStatus() const {
  GLint rc;
  GLsizei length;

  glGetProgramiv(id_, GL_VALIDATE_STATUS, &rc);
  if (rc == 0) {  // Should be rc == GL_TRUE, don't know why this is wrong
    cout << "Shader is valid";
  } else {
    cerr << "Shader is not valid, status: " << rc;
  }
  cout << endl;

  glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &rc);
  if (rc == 0) {
    cout << "No shader info log";
  } else {
    GLchar log[rc];
    glGetProgramInfoLog(id_, rc, &length, log);
    cout << "Shader info log: " << log;
  }
  cout << endl;

  glGetProgramiv(id_, GL_ATTACHED_SHADERS, &rc);
  cout << "Number of attached shaders: " << rc << "\n";

  glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTES, &rc);
  cout << "Number of active attributes: " << rc << "\n";

  GLsizei bufSize;
  glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufSize);
  GLint size;
  GLenum type;
  GLchar attributeName[bufSize];
  for (int i = 0; i < rc; ++i) {
    glGetActiveAttrib(id_, i, bufSize, &length, &size, &type, attributeName);
    cout << "\t" << attributeName << ": " << type << "\n";
  }
  cout << flush;

  glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &rc);
  cout << "Number of active uniforms: " << rc << "\n";

  glGetProgramiv(id_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);
  GLchar uniformName[bufSize];
  for (int i = 0; i < rc; ++i) {
    glGetActiveUniform(id_, i, bufSize, &length, &size, &type, uniformName);
    cout << "\t" << uniformName << ": " << type << "\n";
  }
  cout << flush;
  Renderer::CheckGLError();
}

void Shader::CheckGLError(const string &errorMessage) {
  auto rc = glGetError();
  if (rc != GL_NO_ERROR) {
    cerr << errorMessage << endl;
    throw runtime_error(reinterpret_cast<const char *>(gluErrorString(rc)));
  }
}

void Shader::CheckProgramivError(const GLuint program, const GLenum pname,
                                 const string &errorMessage) {
  GLint rc;
  glGetProgramiv(program, pname, &rc);
  if (rc != GL_TRUE) {
    cerr << errorMessage << endl;
    GLsizei length, buffer_size;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buffer_size);
    GLchar error[buffer_size];
    glGetProgramInfoLog(program, buffer_size, &length,
                        reinterpret_cast<GLchar *>(&error));
    throw runtime_error(error);
  }
}

void Shader::CheckShaderivError(const GLuint shader, const GLenum pname,
                                const string &errorMessage) {
  GLint rc;
  glGetShaderiv(shader, pname, &rc);
  if (rc != GL_TRUE) {
    cerr << errorMessage << endl;
    GLsizei length, buffer_size;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buffer_size);
    GLchar error[buffer_size];
    glGetShaderInfoLog(shader, buffer_size, &length,
                       reinterpret_cast<GLchar *>(&error));
    throw runtime_error(error);
  }
}
