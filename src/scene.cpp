#include "scene.h"
#include <algorithm>
void Scene::render(glslShader & shader, textureManager& manager, Camera * pCamera)
{
	//auto func = std::bind(&Geometry::render, std::placeholders::_1, 222);
	//std::for_each(m_geometryVec.begin(), m_geometryVec.end(), func);
	shader.begin();
	shader.setScene(this);
	shader.setCamera(pCamera);
	auto function = [&shader, &manager](Geometry* pGeometry){pGeometry->render(shader, manager);};
	std::for_each(m_geometryVec.begin(), m_geometryVec.end(), function);
	shader.end();
}
void Scene::init()
{
	m_lightPos = nv::vec3f(20, 20, 20);
	m_geometryVec.resize(m_fileName.size());
	for (int i = 0; i < m_geometryVec.size(); i++)
	{
		m_geometryVec[i] = new Geometry(m_fileName[i].c_str());
		m_geometryVec[i]->Create_GPU_Buffer();
	}
}