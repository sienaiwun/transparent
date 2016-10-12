#include "globaleoc.h"
#include <algorithm>
#include "geometry.h"
#include "quad.h"
#define CHECK_ERRORS()         \
	do {                         \
	GLenum err = glGetError(); \
	if (err) {                                                       \
	printf( "GL Error %d at line %d of FILE %s\n", (int)err, __LINE__,__FILE__);       \
	exit(-1);                                                      \
		}                                                                \
		} while(0)

#include "gbuffershader.h"
EOCrender::EOCrender()
{
	m_eocRightCam = EocCamera(is_Right, 5, 50);
	m_debugSwap = false;
}
EOCrender::EOCrender(int w, int h) :m_height(h), m_width(w), m_pScene(NULL)
{
	m_renderFbo = Fbo(1, m_width, m_height);
	m_renderFbo.init();
	
	m_gbufferFbo = Fbo(3, m_width, m_height);
	m_gbufferFbo.init();
	m_edgeFbo = Fbo(1, m_width, m_height);
	m_edgeFbo.init();
	m_occludedBuffer = Fbo(1, m_width, m_height);
	m_occludedBuffer.init();

	m_edgeShader.init();
	m_gbufferShader.init();

	m_volumnShader.init();
	m_volumnShader.setEdgeFbo(&m_edgeFbo);
	m_volumnShader.setGbuffer(&m_gbufferFbo);
	
	
	m_edgeShader.setGbuffer(&m_gbufferFbo);
	m_edgeShader.setRes(nv::vec2f(m_width, m_height));
	
	m_blendShader.init();
	
	m_pQuad = new QuadScene();

	m_eocRightCam = EocCamera(is_Right, 5, 50);
	m_debugSwap = false;
	CHECK_ERRORS();

}
static bool once = true;;
void EOCrender::render(textureManager & manager)
{
	if (once)
	{
		m_eocRightCam.Look();
		once = false;
	}
	Camera * pRenderCamera;
	if (m_debugSwap == false)
		pRenderCamera = pOriginCam;
	else
		pRenderCamera = m_eocRightCam.getEocCameraP();

	assert(m_pScene != NULL);
	glDisable(GL_CULL_FACE);
	m_gbufferFbo.begin();
	m_gbufferShader.setDiffuse(nv::vec3f(1, 0, 0));
	m_pScene->render(m_gbufferShader, manager, pRenderCamera);
	m_gbufferFbo.end();

	
	m_edgeShader.setCamera(pRenderCamera);
	m_edgeFbo.begin();
	Geometry::drawQuad(m_edgeShader);
	m_edgeFbo.end();
	
	m_occludedBuffer.begin();
	m_volumnShader.setCamera(pOriginCam);
	m_volumnShader.setDiffuse(nv::vec3f(1, 0, 0));
	Geometry::drawTriangle(m_eocRightCam.getEocCameraP()->getCameraPos(), m_volumnShader);
	m_occludedBuffer.end();
	
	
	
	m_blendShader.setBuffer1(&m_gbufferFbo);
	m_blendShader.setBuffer2(&m_occludedBuffer);
	m_renderFbo.begin();
	Geometry::drawQuad(m_blendShader);
	m_renderFbo.end();
	
	
}