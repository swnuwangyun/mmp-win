#include "opengltest.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <SOIL.h>

#include "texthandle.h"
#include "pmx.h"
#include "vmd.h"
#include "shader.h"
#include "pmxvLogger.h"

#include "motioncontroller.h"
#include "bulletphysics.h"
#include "mmdphysics.h"

#include "sound.h"

#include "glfw_func_callbacks.h"
#include <GL/glfw.h>

#include "shader.h"
#include "libtext.h"
#include "libpath.h"

#define WIDTH  1280
#define HEIGHT 720

GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f,  0.5f, 0.0f
};


static std::string __DATA_PATH()
{
	std::wstring wpath = libpath::getThisDllPath();
	std::string path = libtext::wstring2string(wpath);
	return path;
}
#define DATA_PATH	__DATA_PATH()

OpenGLTest* OpenGLTest::m_instance = NULL;

OpenGLTest::OpenGLTest()
{
}

OpenGLTest::~OpenGLTest()
{
	glfwTerminate();
}

OpenGLTest *OpenGLTest::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new OpenGLTest();
	}
	return m_instance;
}

int OpenGLTest::init()
{
	// 窗口创建
	int width = WIDTH;
	int height = HEIGHT;
	if (!glfwInit())
	{
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Don't want old OpenGL

	if (!glfwOpenWindow(width, height, 1.0, 0, 0, 0, 32, 0, GLFW_WINDOW))
	{
		return -2;
	}

	if (glewInit() != GLEW_OK)
	{
		return -3;
	}

	offScreenInit();
	vertInit();
	compileShader();

	while (glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 用于渲染指令
		glUseProgram(m_shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
		glfwSwapBuffers();

		getImage();
	}
}

void OpenGLTest::offScreenInit()
{
	// 创建Offscreen texture与Framebuffer
	int dst_w = WIDTH;
	int dst_h = HEIGHT;

	// create texture2D
	glCreateTextures(GL_TEXTURE_2D, 1, &m_offscreenTexture);
	glTextureStorage2D(m_offscreenTexture, 0, GL_RGBA8, dst_w, dst_h); // 为texture分配内存

	GLuint framebuf;
	glCreateFramebuffers(1, &framebuf);

	// bind texture to Framebuffer
	glNamedFramebufferTexture1DEXT(framebuf, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_offscreenTexture, 0);

	// 渲染到Offscreen texture上
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuf);
	// viewport设置成与texture大小一致
	glViewport(0, 0, dst_w, dst_h);

	// 重新将窗口默认Framebuffer激活
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "Framebuffer complete." << std::endl;
		return;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
		return;


	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
		return;
		/*
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
		return false;


		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
		std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
		return false;
		*/
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
		return;


	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
		return;


	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
		return;


	default:
		std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
		return;
	}
}

void OpenGLTest::vertInit()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// 2. 把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//4. 解绑VAO
	glBindVertexArray(0);
}

GLuint OpenGLTest::compileShader()
{
	std::string vertexShaderPath = DATA_PATH + "/shaders/opengltest.vert";
	std::string fragmentShaderPath = DATA_PATH + "/shaders/opengltest.frag";
	m_shaderProgram = compileShaders(vertexShaderPath, fragmentShaderPath);

	linkShaders(m_shaderProgram);
	return m_shaderProgram;
}

bool OpenGLTest::getImage()
{
	// 将opengl texture中的内容拷贝到内存中
	return true;
}