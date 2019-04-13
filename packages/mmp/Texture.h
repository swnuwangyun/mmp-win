#pragma once
#include "utils.h"

class Texture
{
public:
	Texture();
	~Texture();

	void init(bool bIsDllCall);
	void draw(glm::mat4 & viewMatrix, glm::mat4 & projectionMatrix);

private:
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_program;
	GLint m_positionLocation;
	GLint m_modelMatrixLocation;
	GLint m_viewMatrixLocation;
	GLint m_projectionMatrixLocation;
	GLint m_colorLocation;
	GLint m_texcoordLocation;
	GLint m_textureLocation;
	GLuint m_texture;
};

