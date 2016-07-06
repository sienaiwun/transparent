#include "glslShaderLoader.h"
#include <string>
#include "texturemanager.h"
#include<nvMath.h>
#include "glm.h"
#include "Camera.h"

#ifndef GLSLSHADER_H
#define GLSLSHADER_H

class Camera;
class Scene;
class glslShader
{
public:
	virtual void init() = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void setCamera(Camera* pCamera){};
	virtual void setScene(Scene * pScene){};
	virtual void setGeomtryIndex(int index){};
	virtual void setMaterial(const GLMmaterial & meterial, textureManager & manager) = 0;
protected:
	std::string m_vertexFileName,m_geometryFileName,m_fragmentFileName;
	glslShaderLoader m_loader;

};
#endif