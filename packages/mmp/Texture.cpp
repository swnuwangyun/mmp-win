#include "Texture.h"
#include "utils.h"

Texture::Texture()
	:m_vbo(0)
	, m_ebo(0)
	, m_program(0)
	, m_positionLocation(0)
	, m_modelMatrixLocation(0)
	, m_viewMatrixLocation(0)
	, m_projectionMatrixLocation(0)
	, m_colorLocation(0)
	, m_texcoordLocation(0)
	, m_textureLocation(0)
	, m_texture(0)
{
}


Texture::~Texture()
{
}

void Texture::init(void)
{
	float data[] = {
		-100.0f,-10.0f,100.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,
		100.0f,-10.0f,100.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f,
		100.0f,-10.0f,-100.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
		-100.0f,-10.0f,-100.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f,1.0f
	};
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 40, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	unsigned short indexes[] = { 0,1,2,0, 2, 3 };
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6, indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	int fileSize = 0;
	string strPath = utils::appDirA();
	string strFile = strPath + "\\shaders\\test.vs";
	unsigned char * shaderCode = utils::LoadFileContent(strFile.c_str(), fileSize);
	GLuint vsShader = utils::CompileShader(GL_VERTEX_SHADER, (char*)shaderCode);
	delete shaderCode;

	strFile = strPath + "\\shaders\\test.fs";
	shaderCode = utils::LoadFileContent(strFile.c_str(), fileSize);
	GLuint fsShader = utils::CompileShader(GL_FRAGMENT_SHADER, (char*)shaderCode);
	delete shaderCode;

	m_program = utils::CreateProgram(vsShader, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);

	m_positionLocation = glGetAttribLocation(m_program, "position");
	m_colorLocation = glGetAttribLocation(m_program, "color");
	m_texcoordLocation = glGetAttribLocation(m_program, "texcoord");
	m_modelMatrixLocation = glGetUniformLocation(m_program, "ModelMatrix");
	m_viewMatrixLocation = glGetUniformLocation(m_program, "ViewMatrix");
	m_projectionMatrixLocation = glGetUniformLocation(m_program, "ProjectionMatrix");
	m_textureLocation = glGetUniformLocation(m_program, "U_Texture");

	strFile = strPath + "\\shaders\\ground.bmp";
	m_texture = utils::CreateTexture2DFromBMP(strFile.c_str());
}

void Texture::draw(glm::mat4 & viewMatrix, glm::mat4 & projectionMatrix)
{
	glm::mat4 modelMatrix;
	glUseProgram(m_program);
	glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glUniform1i(m_textureLocation, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_positionLocation);
	glVertexAttribPointer(m_positionLocation, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);
	glEnableVertexAttribArray(m_colorLocation);
	glVertexAttribPointer(m_colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(sizeof(float) * 4));
	glEnableVertexAttribArray(m_texcoordLocation);
	glVertexAttribPointer(m_texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(sizeof(float) * 8));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}