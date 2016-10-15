
#include <Windows.h>
#include <driver_types.h>
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <assert.h>
#include <cuda_gl_interop.h>
//#include <cutil_gl_inline.h>
#include <helper_cuda_gl.h>
//#include <cutil_math.h>
//#include <helper_math.h>
//#include <curand_kernel.h>
#ifndef CUDA_H
#define  CUDA_H
//#define DEBUG
#define ROWLARGER (1.2)
enum cudaTexType
{
	occluderbuffer_t,
	edgebuffer_t,
	color_t,
};


class CudaKernel
{
public:
	CudaKernel(int w, int h) :m_height(h), m_width(w)
	{
	}
	void init();
	void run();
private:
	int m_height, m_width;


};
class CudaLinkList
{
public:
	CudaLinkList(int height, int k);
	void* pMemery;
private:
	int actomic;
	int size;

};
class CudaTexResourse
{
public:

	inline void set(int texId, int w, int h, cudaTexType t)
	{
		m_texture = texId;
		m_width = w;
		m_height = h;
		m_type = t;
	}
	void init();
	void map();
	void unmap();
	void setEveryTex(int);
	void set(int);
	inline cudaTexType getType()
	{
		return m_type;
	}
	inline cudaGraphicsResource ** getResPoint()
	{
		return &m_CudaReourse;
	}
	inline int getWidth() const
	{
		return m_width;
	}
	inline int getHeight() const
	{
		return m_height;
	}
private:
	GLuint m_texture;
	GLuint m_tempTex;
	int m_width, m_height;
	cudaGraphicsResource * m_CudaReourse;
	cudaTexType m_type;
};

enum cudaPboType
{
	float4_t,
	float2_t,
	test_t,
	diff_normal_t,
	unit_3,
#ifdef NOISEMAP
	refract_t,

#endif
};
class CudaPboResource
{
public:
	inline void set(int w, int h, cudaPboType t)
	{
		m_width = w;
		m_height = h;
		m_type = t;
	}
	inline cudaPboType getType()
	{
		return m_type;
	}
	void init();
	void map();
	void unMap();
	inline GLuint getTexture()
	{
		assert(float4_t == m_type);
		return m_texture;
	}
	inline cudaGraphicsResource ** getResPoint()
	{
		return &m_CudaReourse;
	}
	void generateTex();
	inline int getWidth() const
	{
		return m_width;
	}
	inline int getHeight() const
	{
		return m_height;
	}
private:
	cudaPboType m_type;
	void initTex();
	void initPbo();
	GLuint m_pbo, m_texture;
	int m_width, m_height;
	cudaGraphicsResource * m_CudaReourse;
};


extern "C" void cudaRelateTex(CudaTexResourse * pResouce);
extern "C" void cudaRelateArray(CudaPboResource * pResouce);
extern "C" void countRow(int width, int height);
extern void cudaInit(int height,int width, int k,int rowLarger);

#endif