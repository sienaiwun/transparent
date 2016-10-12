#include "geometry.h"
#include "glslShader.h"
#include "textureManager.h"
#include <string>
#include <vector>
#include "Camera.h"
#ifndef SCENE_H
#define SCENE_H
class Scene
{
public:
	Scene()
	{
		m_cameraFile = std::string("");
	}
	virtual void init();
	virtual void render(glslShader & shader, textureManager& manager, Camera * pCamera);
	inline nv::vec3f getLightPos()
	{
		return m_lightPos;
	}
	void LoadCamera(Camera * pCamera)
	{
		if (m_cameraFile.compare("") == 0)
		{

		}
		else
		{
			pCamera->loadToFIle(m_cameraFile.c_str());
		}
	}
	std::string getCameraFile() const
	{
		return m_cameraFile;
	}
protected:
	vector<Geometry*> m_geometryVec;
	vector<string> m_fileName;
	nv::vec3f m_lightPos;
	std::string m_cameraFile;
};
#endif