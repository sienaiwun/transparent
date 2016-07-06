#include "geometry.h"
#include "glslShader.h"
#include "textureManager.h"

#include <vector>
#ifndef SCENE_H
#define SCENE_H
class Scene
{
public:
	virtual void init();
	virtual void render(glslShader & shader, textureManager& manager, Camera * pCamera);
	inline nv::vec3f getLightPos()
	{
		return m_lightPos;
	}
protected:
	vector<Geometry*> m_geometryVec;
	vector<string> m_fileName;
	nv::vec3f m_lightPos;
};
#endif