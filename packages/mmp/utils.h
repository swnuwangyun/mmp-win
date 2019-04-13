#pragma once
//¹«¹²º¯Êý¿â.

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace std;

namespace utils
{
	wstring appDir(void);
	string appDirA(void);
	unsigned char * LoadFileContent(const char*path, int&filesize);
	GLuint CompileShader(GLenum shaderType, const char*shaderCode);
	GLuint CreateProgram(GLuint vsShader, GLuint fsShader);
	unsigned char * DecodeBMP(unsigned char*bmpFileData, int&width, int&height);
	GLuint CreateTexture2D(unsigned char*pixelData, int width, int height, GLenum type);
	GLuint CreateTexture2DFromBMP(const char *bmpPath);
	GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data = nullptr);
}