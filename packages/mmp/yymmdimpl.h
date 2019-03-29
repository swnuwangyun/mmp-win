#pragma once
#include "IYYMMD.h"
#include "QcCriticalLock.h"

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GL/glfw.h>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>

#define BUFFER_OFFSET(offset) ((void *) (offset))

enum VAO_IDs { Vertices, NumVAOs };
enum Buffer_IDs { VertexArrayBuffer, VertexIndexBuffer, RecordBuffer, NumBuffers };
enum Attrib_IDs { vPosition, vUV, vNormal, vBoneIndices, vBoneWeights, vWeightFormula };
enum Uniform_IDs {
	uAmbient, uDiffuse, uSpecular, uShininess,
	uIsEdge, uEdgeColor, uEdgeSize,
	uHalfVector, uLightDirection,
	uSphereMode,
	uTextureSampler, uSphereSampler, uToonSampler,
	NumUniforms
};

namespace ClosedMMDFormat
{
	struct PMXInfo;
	struct VMDInfo;
}
class VMDMotionController;
class BulletPhysics;
class MMDPhysics;
class VertexData;

class YYMMDImpl : public IYYMMD
{
public:
	YYMMDImpl();
	virtual ~YYMMDImpl();
	static YYMMDImpl *GetInstance();

	virtual bool init(const char* version);
	virtual void setModelPath(const char* modePath, const char* motionPath, const char* musicPath);
	virtual void render();
	virtual void setBoneData(const float* data);
	virtual void copyOfTextureData(char* dst);
	virtual void setLogPath(const char* logPath);
	virtual bool unInit();

private:
	void resetPmxVmd();
	void hackShaderFiles();
	void createOffscreenRendering();
	void loadTextures();
	void initBuffers();
	void initUniformVarLocations();
	void setCamera(GLuint MVPLoc);
	std::string hackShaderFile(std::string filename);

private:
	ClosedMMDFormat::PMXInfo* m_pmxInfo;
	ClosedMMDFormat::VMDInfo* m_vmdInfo;
	VMDMotionController*      m_motionController;
	QcCriticalLock			  m_pmxvmdlock;

	int GLVersionHintMajor, GLVersionHintMinor;
	int GLVersionMajor, GLVersionMinor, GLVersionRevision;

	std::string			m_vertShaderPath;
	std::string			m_fragShaderPath;
	GLuint				m_shaderProgram;
	std::string			m_bulletVertPath;
	std::string			m_bulletFragPath;
	std::vector<GLuint> m_textures;

	GLuint				VAOs[NumVAOs];
	GLuint				Buffers[NumBuffers];
	GLuint				uniformVars[NumUniforms];

	BulletPhysics*		m_bulletPhysics;
	MMDPhysics*			m_mmdPhysics;
	glm::vec3			m_modelTranslate;

	GLuint				m_imageFBO;
	GLuint				m_offscreenTexture;
	GLuint				m_depthTextureID;

	GLuint MVP_loc;

	static YYMMDImpl *m_instance;
};