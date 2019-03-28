#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GL/glfw.h>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>

class OpenGLTest
{
public:
	OpenGLTest();
	virtual ~OpenGLTest();
	static OpenGLTest *GetInstance();

	int init();

private:
	void offScreenInit();
	void vertInit();
	GLuint compileShader();
	bool getImage();

private:
	static OpenGLTest *m_instance;
	GLuint m_shaderProgram;
	GLuint VAO, VBO;
	GLuint m_offscreenTexture;
};