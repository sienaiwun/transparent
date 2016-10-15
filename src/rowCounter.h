#ifndef ROWCOUNTER_H
#define ROWCOUNTER_H
#include "Fbo.h"
#include "cuda.h"

class RowCounter
{
public:
	RowCounter() = default;
	RowCounter(int w, int h, int k = 10);
	~RowCounter() = default;
	inline void setGbuffer(Fbo * pFbo)
	{
		pGbuffer = pFbo;
	}
	inline void setOccludorBuffer(Fbo * pFbo)
	{
		pOccluderFbo = pFbo;
	}
	inline void setEdgeBuffer(Fbo * pFbo)
	{
		pEdgeFbo = pFbo;
	}
	inline GLuint getTex() 
	{
		return m_getTex;
	}
	void init();
	void render();
private:
	const int m_width, m_height,m_k;
	GLuint m_getTex;
	Fbo * pGbuffer;
	Fbo * pOccluderFbo;
	Fbo * pEdgeFbo;
	GLuint m_head_pointer_texture, m_head_pointer_initializer, m_atomic_counter_buffer, m_fragment_storage_buffer;
	GLuint *m_data;
	GLuint m_test;
	CudaTexResourse *m_colorCudaTex, *m_edgeCudaTex, *m_occluderCudaTex;
	CudaPboResource *m_initArray,*m_OutTex;

};
#endif
