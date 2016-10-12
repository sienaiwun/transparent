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

	m_edgeShader.init();
	m_gbufferShader.init();
	
	m_edgeShader.setGbuffer(&m_gbufferFbo);
	m_edgeShader.setRes(nv::vec2f(m_width, m_height));
	

	m_pQuad = new QuadScene();

	m_eocRightCam = EocCamera(is_Right, 5, 50);
	m_eocTopCam = EocCamera(is_Top, 5, 50);
	m_debugSwap = false;
	CHECK_ERRORS();

}

void EOCrender::render(textureManager & manager)
{
	m_eocRightCam.Look();
	m_eocTopCam.Look();
	Camera * pRenderCamera;
	if (m_debugSwap == false)
		pRenderCamera = pOriginCam;
	else
		pRenderCamera = m_eocTopCam.getEocCameraP();
	assert(m_pScene != NULL);
	glDisable(GL_CULL_FACE);
	m_gbufferFbo.begin();
	m_pScene->render(m_gbufferShader, manager, pRenderCamera);
	m_gbufferFbo.end();
	

	m_edgeShader.setCamera(pRenderCamera);
	m_edgeFbo.begin();
	Geometry::drawQuad(m_edgeShader);
	m_edgeFbo.end();

	
	
}