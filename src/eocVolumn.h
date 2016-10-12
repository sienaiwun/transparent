#include "glslShader.h"
#include "Fbo.h"
#ifndef EOCVOLUMN_H
#define EOCVOLUMN_H
class EocVolumnShader :public glslShader
{
public:
	EocVolumnShader()
	{
		m_vertexFileName = "Shader/eocVolumn.vert";
		m_geometryFileName = "Shader/eocVolumn.geom";
		m_fragmentFileName = "Shader/eocVolumn.frag";
	}
	virtual void init();
	virtual void begin();
	virtual void end();
	virtual void setCamera(Camera * pCamera)
	{
		m_pCamera = pCamera;
	}
	virtual void setScene(Scene * pScene);
	virtual void setGeomtryIndex(int i);
	virtual void setMaterial(const GLMmaterial & meterial, textureManager & manager);
	inline void setEdgeFbo(Fbo * pFbo)
	{
		m_pEdgeFbo = pFbo;
	}
	inline void setGbuffer(Fbo * pFbo)
	{
		m_pGbuffer = pFbo;
	}
	void setPara();
	inline void setDiffuse(nv::vec3f color)
	{
		m_diffuseColor = color;
	}
private:

	GLuint m_vmpBinding;
	GLuint m_modelViewBinding;
	float* m_mvp;
	float* m_modelView;
	GLuint m_posTexSlot;
	GLuint m_objectDiffuseBinding;
	GLuint m_cameraPosBinding;
	GLuint m_lightPosBinding;
	GLuint m_hasTex;
	GLuint m_objectId;
	GLuint m_reflectFactor;
	Fbo * m_pGbuffer;
	Camera * m_pCamera;
	Fbo * m_pEdgeFbo;
	nv::vec3f m_diffuseColor;
	nv::vec3f m_lightPos;
	nv::vec3f m_cameraPos;


};
#endif