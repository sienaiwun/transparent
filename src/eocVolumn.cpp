#include "eocVolumn.h"
#include "scene.h"

void EocVolumnShader::init()
{
	m_loader.loadShader(m_vertexFileName.c_str(), 0, m_fragmentFileName.c_str());
	m_vmpBinding = m_loader.getUniform("MVP");
	m_modelViewBinding = m_loader.getUniform("modelView");
	m_cameraPosBinding = m_loader.getUniform("cameraPos");
	m_lightPosBinding = m_loader.getUniform("lightPos");
	m_objectDiffuseBinding = m_loader.getUniform("objectDiffuseColor");
	m_posTexSlot = m_loader.getUniform("posTex");
	m_hasTex = m_loader.getUniform("hasTex");
	m_objectId = m_loader.getUniform("objectId");
	m_reflectFactor = m_loader.getUniform("reflectFactor");

}

void EocVolumnShader::setScene(Scene * pScene)
{
	glUniform3fv(m_lightPosBinding, 1, (float*)pScene->getLightPos());
}
void EocVolumnShader::setGeomtryIndex(int i)
{
	glUniform1i(m_objectId, i);

}
void EocVolumnShader::begin()
{
	m_loader.useShder();
	glUniform3fv(m_objectDiffuseBinding, 1, m_diffuseColor);
	setPara();
}
void EocVolumnShader::end()
{
	m_loader.DisUse();

}

void EocVolumnShader::setMaterial(const GLMmaterial & material, textureManager & manager)
{
	//if (material.ambient_map[0] == 'a');
}
void EocVolumnShader::setPara()
{
	m_mvp = m_pCamera->getMvpMat();
	m_modelView = m_pCamera->getModelViewMat();
	glUniformMatrix4fv(m_vmpBinding, 1, GL_FALSE, m_mvp);
	glUniformMatrix4fv(m_modelViewBinding, 1, GL_FALSE, m_modelView);
	glUniform3f(m_cameraPosBinding, m_pCamera->getCameraPos().x, m_pCamera->getCameraPos().y, m_pCamera->getCameraPos().z);
	setShaderTex(m_posTexSlot, m_pGbuffer->getTexture(1));
	resetTexId();

}