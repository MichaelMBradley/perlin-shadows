#include "shader.h"

#include <fstream>
#include <iostream>
#include <string>

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

Shader::~Shader() { glDeleteProgram(id_); }

bool Shader::CopyDataToUniform(const glm::mat4 &data,
                               const string &name) const {
  auto location = glGetUniformLocation(id_, name.c_str());
  if (location == -1) {
    return false;
  }

  glUniformMatrix4fv(location, 1, GL_FALSE, &data[0][0]);
  return true;
}

bool Shader::CopyDataToUniform(const glm::vec4 &data,
                               const string &name) const {
  auto location = glGetUniformLocation(id_, name.c_str());
  if (location == -1) {
    return false;
  }
  glUniform4fv(location, 1, &data[0]);
  return true;
}

bool Shader::CopyDataToUniform(const glm::vec3 &data,
                               const string &name) const {
  auto location = glGetUniformLocation(id_, name.c_str());
  if (location == -1) {
    return false;
  }
  glUniform3fv(location, 1, &data[0]);
  return true;
}

bool Shader::CopyDataToUniform(float data, const string &name) const {
  auto location = glGetUniformLocation(id_, name.c_str());
  if (location == -1) {
    return false;
  }
  glUniform1fv(location, 1, &data);
  return true;
}

bool Shader::CopyDataToUniform(int data, const string &name) const {
  auto location = glGetUniformLocation(id_, name.c_str());
  if (location == -1) {
    return false;
  }
  glUniform1iv(location, 1, &data);
  return true;
}

bool Shader::CopyDataToUniform(bool data, const string &name) const {
  auto location = glGetUniformLocation(id_, name.c_str());
  if (location == -1) {
    return false;
  }
  int actualData = static_cast<int>(data);
  glUniform1iv(location, 1, &actualData);
  return true;
}

string Shader::ReadFile(const string &filename) {
  string data, line;
  ifstream file;
  file.open(filename, std::ios::binary);
  if (file.is_open()) {
    while (getline(file, line)) {
      data += line + '\n';
    }
    if (data.empty()) {
      data.pop_back();
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

  glValidateProgram(id_);
  glGetProgramiv(id_, GL_VALIDATE_STATUS, &rc);
  if (rc == GL_TRUE) {
    cout << "Shader is valid" << endl;
  } else {
    cerr << "Shader is not valid, status: " << rc << endl;
  }

  glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &rc);
  if (rc == 0) {
    cout << "No shader info log";
  } else {
    auto *log = new GLchar[rc];
    glGetProgramInfoLog(id_, rc, &length, log);
    cout << "Shader info log: " << log;
    delete[] log;
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
  auto attributeName = new GLchar[bufSize];
  for (int i = 0; i < rc; ++i) {
    glGetActiveAttrib(id_, i, bufSize, &length, &size, &type, attributeName);
    cout << "\t" << attributeName << ": " << type << "\n";
  }
  delete[] attributeName;
  cout << flush;

  glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &rc);
  cout << "Number of active uniforms: " << rc << "\n";

  glGetProgramiv(id_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);
  auto *uniformName = new GLchar[bufSize];
  for (int i = 0; i < rc; ++i) {
    glGetActiveUniform(id_, i, bufSize, &length, &size, &type, uniformName);
    cout << "\t" << uniformName << ": " << type << "\n";
  }
  delete[] uniformName;
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
    GLsizei length, bufSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);
    auto error = new GLchar[bufSize];
    glGetProgramInfoLog(program, bufSize, &length, error);
    throw runtime_error(error);
  }
}

void Shader::CheckShaderivError(const GLuint shader, const GLenum pname,
                                const string &errorMessage) {
  GLint rc;
  glGetShaderiv(shader, pname, &rc);
  if (rc != GL_TRUE) {
    cerr << errorMessage << endl;
    GLsizei length, bufSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);
    auto error = new GLchar[bufSize];
    glGetShaderInfoLog(shader, bufSize, &length, error);
    throw runtime_error(error);
  }
}
