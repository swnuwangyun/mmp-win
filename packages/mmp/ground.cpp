#include "ground.h"

void Ground::Init() {
	mVertexBuffer = new VertexBuffer;
	mVertexBuffer->SetSize(1600);
	for (int z = 0; z < 20; ++z) {
		float zStart = 100.0f - z * 10.0f;
		for (int x = 0; x < 20; ++x) {
			int offset = (x + z * 20) * 4;
			float xStart = x * 10.0f - 100.0f;
			mVertexBuffer->SetPosition(offset, xStart, -1.0f, zStart);
			mVertexBuffer->SetPosition(offset + 1, xStart + 10.0f, -1.0f, zStart);
			mVertexBuffer->SetPosition(offset + 2, xStart, -1.0f, zStart - 10.0f);
			mVertexBuffer->SetPosition(offset + 3, xStart + 10.0f, -1.0f, zStart - 10.0f);
			mVertexBuffer->SetNormal(offset, 0.0f, 1.0f, 0.0f);
			mVertexBuffer->SetNormal(offset + 1, 0.0f, 1.0f, 0.0f);
			mVertexBuffer->SetNormal(offset + 2, 0.0f, 1.0f, 0.0f);
			mVertexBuffer->SetNormal(offset + 3, 0.0f, 1.0f, 0.0f);
			if ((z % 2) ^ (x % 2)) {
				mVertexBuffer->SetColor(offset, 0.1f, 0.1f, 0.1f);
				mVertexBuffer->SetColor(offset + 1, 0.1f, 0.1f, 0.1f);
				mVertexBuffer->SetColor(offset + 2, 0.1f, 0.1f, 0.1f);
				mVertexBuffer->SetColor(offset + 3, 0.1f, 0.1f, 0.1f);
			}
			else {
				mVertexBuffer->SetColor(offset, 0.8f, 0.8f, 0.8f);
				mVertexBuffer->SetColor(offset + 1, 0.8f, 0.8f, 0.8f);
				mVertexBuffer->SetColor(offset + 2, 0.8f, 0.8f, 0.8f);
				mVertexBuffer->SetColor(offset + 3, 0.8f, 0.8f, 0.8f);
			}
		}
	}
	mVBO = utils::CreateBufferObject(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertexBuffer->mVertexCount, GL_STATIC_DRAW, mVertexBuffer->mVertexes);
	mShader = new Shader;
	string strVS = utils::appDirA() + "\\shaders\\ground.vs";
	string strFS = utils::appDirA() + "\\shaders\\ground.fs";
	mShader->Init(strVS.c_str(), strFS.c_str());
}

void Ground::Draw(glm::mat4 & viewMatrix, glm::mat4 & projectionMatrix) {
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glUseProgram(mShader->mProgram);
	mShader->Bind(glm::value_ptr(mModelMatrix), glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix));
	for (int i = 0; i < 400; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glUseProgram(0);
}