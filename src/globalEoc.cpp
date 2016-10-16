#include "globaleoc.h"
#include <algorithm>
#include "geometry.h"
#include "quad.h"
#define GL_CONSERVATIVE_RASTERIZATION_NV 0x9346
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
	glBindTexture(GL_TEXTURE_2D, m_edgeFbo.getTexture(0));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D,0);
	m_occludedBuffer = Fbo(1, m_width, m_height);
	m_occludedBuffer.init();

	m_edgeShader.init();
	m_gbufferShader.init();

	m_volumnShader.init();
	m_volumnShader.setRes(nv::vec2f(m_width, m_height));
	
	
	m_edgeShader.setGbuffer(&m_gbufferFbo);
	m_edgeShader.setRes(nv::vec2f(m_width, m_height));
	
	m_blendShader.init();
	
	m_pQuad = new QuadScene();
	m_eocRightCam = EocCamera(is_Right, 0.1, 50);
	m_debugSwap = false;
	
	pCounter = new RowCounter(w, h);
	pCounter->setGbuffer(&m_gbufferFbo);
	pCounter->setOccludorBuffer(&m_occludedBuffer);

	pCounter->setEdgeBuffer(&m_edgeFbo);
	pCounter->init();
}
static bool once = true;;
void EOCrender::render(textureManager & manager)
{
	//pCounter->render();
	if (once)
	{
		m_eocRightCam.Look();
		//once = false;
	}
	Camera * pRenderCamera;
	if (m_debugSwap == false)
		pRenderCamera = pOriginCam;
	else
		pRenderCamera = m_eocRightCam.getEocCameraP();

	assert(m_pScene != NULL);
	m_gbufferFbo.begin();
	m_gbufferShader.setDiffuse(nv::vec3f(1, 0, 0));
	m_pScene->render(m_gbufferShader, manager, pRenderCamera);
	//m_gbufferFbo.SaveBMP("gbuffer.bmp", 0);
	m_gbufferFbo.end();

	//glCullFace(GL_BACK);
	glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
	m_edgeShader.setCamera(pRenderCamera);
	m_edgeFbo.begin();
	m_pScene->render(m_edgeShader, manager, pRenderCamera);
	//Geometry::drawQuad(m_edgeShader);
	//m_edgeFbo.SaveBMP("edge.bmp", 0);
	//m_edgeFbo.debugPixel(0, 719, 533);
	//m_edgeFbo.debugPixel(0,603, 713);
	m_edgeFbo.end();
	glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
	
	m_volumnShader.setCamera(pOriginCam);
	m_volumnShader.setGbuffer(&m_gbufferFbo);
	m_volumnShader.setDiffuse(nv::vec3f(1, 0, 0));
	m_volumnShader.setEdgeFbo(&m_edgeFbo);
	m_volumnShader.setEocCamera(m_eocRightCam.getEocCameraP()->getCameraPos());
	m_occludedBuffer.begin();
	 
	m_pScene->render(m_volumnShader, manager, pRenderCamera);
	//m_occludedBuffer.SaveBMP("test.bmp", 0);
	m_occludedBuffer.end();
	
	 


	m_blendShader.setBuffer1(&m_gbufferFbo);
	m_blendShader.setBuffer2(&m_edgeFbo);
	 
	m_renderFbo.begin();
	Geometry::drawQuad(m_blendShader);
//	m_renderFbo.SaveBMP("test.bmp", 0);

	m_renderFbo.end();
	
	pCounter->render();
	
}