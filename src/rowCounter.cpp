#include "rowCounter.h"
#include <algorithm>
#include "assert.h"

#define CHECK_ERRORS()         \
	do {                         \
	GLenum err = glGetError(); \
	if (err) {                                                       \
	printf( "GL Error %d at line %d of FILE %s\n", (int)err, __LINE__,__FILE__);       \
	exit(-1);                                                      \
			}                                                                \
			} while(0)


static GLuint getAtomicCounter(GLuint buffer)
{
	// read back total fragments that been shaded
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
	GLuint *ptr = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);
	GLuint fragsCount = *ptr;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	return fragsCount;
}
void RowCounter::init()
{
	assert(pGbuffer != NULL);
	assert(pEdgeFbo != NULL);
	assert(pOccluderFbo != NULL);
	m_colorCudaTex = new CudaTexResourse();
	m_colorCudaTex->set(pGbuffer->getTexture(0), m_width, m_height, color_t);
	m_colorCudaTex->init();
	m_edgeCudaTex = new  CudaTexResourse();
	m_edgeCudaTex->set(pEdgeFbo->getTexture(0), m_width, m_height, edgebuffer_t);
	m_edgeCudaTex->init();
	m_occluderCudaTex = new  CudaTexResourse();
	m_occluderCudaTex->set(pOccluderFbo->getTexture(0), m_width, m_height, occluderbuffer_t);
	m_occluderCudaTex->init();

	m_initArray = new CudaPboResource();
	m_initArray->set(1, m_height, unit_4);
	m_initArray->init();
	m_OutTex = new CudaPboResource();
	m_OutTex->set(1.5*m_width, m_height, float4_t);
	m_OutTex->init();
	cudaInit(m_height, m_k);


}

RowCounter::RowCounter(int w, int h, int k) :m_width(w), m_height(h), m_k(k)
{
	pGbuffer = NULL;
	pEdgeFbo = NULL;
	pOccluderFbo = NULL;

}
void RowCounter::render()
{
	m_OutTex->map();
	m_initArray->map();
	m_occluderCudaTex->map();
	m_edgeCudaTex->map();
	m_colorCudaTex->map();

	countRow(m_width, m_height);
	m_colorCudaTex->unmap();
	m_edgeCudaTex->unmap();
	m_occluderCudaTex->unmap();
	m_initArray->unMap();
	m_OutTex->unMap();
	m_OutTex->generateTex();
	m_getTex = m_OutTex->getTexture();
}