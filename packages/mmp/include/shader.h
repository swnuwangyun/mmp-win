#ifndef SHADER_H
#define SHADER_H

#include <iostream>

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GL/glfw.h>

GLuint compileShaders(std::string vertShaderName, std::string fragShaderName);
void linkShaders(GLuint program);

#endif
