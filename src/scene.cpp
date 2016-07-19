#include "scene.h"
#include <algorithm>

#define CHECK_ERRORS()         \
	do {                         \
	GLenum err = glGetError(); \
	if (err) {                                                       \
	printf( "GL Error %d at line %d of FILE %s\n", (int)err, __LINE__,__FILE__);       \
	exit(-1);                                                      \
		}                                                                \
		} while(0)

void Scene::render(glslShader & shader, textureManager& manager, Camera * pCamera)
{
	//auto func = std::bind(&Geometry::render, std::placeholders::_1, 222);
	//std::for_each(m_geometryVec.begin(), m_geometryVec.end(), func);
	shader.begin();
	CHECK_ERRORS();
	shader.setScene(this);
	CHECK_ERRORS();
	shader.setCamera(pCamera);
	CHECK_ERRORS();
	auto function = [&shader, &manager](Geometry* pGeometry){pGeometry->render(shader, manager);};
	CHECK_ERRORS();
	std::for_each(m_geometryVec.begin(), m_geometryVec.end(), function);
	CHECK_ERRORS();
	shader.end();
	CHECK_ERRORS();
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