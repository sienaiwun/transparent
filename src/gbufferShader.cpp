#include "gBufferShader.h"
#include "scene.h"

void GbufferShader::init()
{
	m_loader.loadShader(m_vertexFileName.c_str(), 0, m_fragmentFileName.c_str());
	m_vmpBinding = m_loader.getUniform("MVP");
	m_modelViewBinding = m_loader.getUniform("modelView");
	m_cameraPosBinding = m_loader.getUniform("cameraPos");
	m_lightPosBinding = m_loader.getUniform("lightPos");
	m_objectDiffuseBinding = m_loader.getUniform("objectDiffuseColor");
	m_objectTexBinding = m_loader.getUniform("objectTex");
	m_hasTex = m_loader.getUniform("hasTex");
	m_objectId = m_loader.getUniform("objectId");
	m_reflectFactor = m_loader.getUniform("reflectFactor");

}

void GbufferShader::setScene(Scene * pScene)
{
	glUniform3fv(m_lightPosBinding, 1, (float*)pScene->getLightPos());
}
void GbufferShader::setGeomtryIndex(int i)
{
	glUniform1i(m_objectId, i);

}
void GbufferShader::begin()
{
	m_loader.useShder();
}
void GbufferShader::end()
{
	m_loader.DisUse();
}
void GbufferShader::setMaterial(const GLMmaterial & material, textureManager & manager)
{
	glUniform3fv(m_objectDiffuseBinding, 1, material.diffuse);
	GLuint  texid = manager.getTexId(material.diffuse_map);
	printf("%s\n", material.diffuse_map);
	if (texid > 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texid);
		glUniform1i(m_objectTexBinding, 0);
	}
	//if (material.ambient_map[0] == 'a');

}
void GbufferShader::setCamera(Camera *pCamera)
{
	m_mvp = pCamera->getMvpMat();
	m_modelView = pCamera->getModelViewMat();
	glUniformMatrix4fv(m_vmpBinding, 1, GL_FALSE, m_mvp);
	glUniformMatrix4fv(m_modelViewBinding, 1, GL_FALSE, m_modelView);
	glUniform3f(m_cameraPosBinding, pCamera->getCameraPos().x, pCamera->getCameraPos().y, pCamera->getCameraPos().z);
}