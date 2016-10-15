#include "cuda.h"

texture<float4, 2, cudaReadModeElementType> cudaEdgeTex;
texture<float4, 2, cudaReadModeElementType> cudaOccuderTex;
texture<float4, 2, cudaReadModeElementType> cudaColorTex;
cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 32, 32, 32, cudaChannelFormatKindFloat);

__device__ bool isVolume(float2 uv, int *state)
{
	if (uv.x > 230 && uv.x < 300)
		return true;
	float4 value = tex2D(cudaOccuderTex, uv.x, uv.y);
	return value.x > 0.5;
}
typedef enum {
	isVolumn,
	notVolumn,
}pixelEnum;
class ListNote
{
public:

	unsigned int beginIndex, endIndex, nextPt;
};

uint4 *cuda_PBO_Buffer;

__device__ uint4* d_cudaPboBuffer;
float4 *cuda_TexturePbo_buffer;
__device__ float4* d_cudaTexture;
__device__ int imageWidth, imageHeight, d_outTextureWidth, d_outTextureHeigh;
__device__ int d_index;
__device__ ListNote* d_listBuffer;
__device__ int d_atomic;

class List
{
	
};
__global__ void countRowKernel(int kernelWidth, int kernelHeight)
{
	int y = __umul24(blockIdx.y, blockDim.y) + threadIdx.y;
	if ( y > kernelHeight)
		return;
	//if (y != 599)
	//	return;
	int arrayNum = y;
	int accumNum = 0;
	int state = 0;
	pixelEnum etype = notVolumn;
	unsigned int* nextPtr = &d_cudaPboBuffer[arrayNum].x;
	int listIndex;
	for (int x = 0; x < imageWidth;x++)
	{
		float2 currentUv = make_float2(x + 0.5, y + 0.5);
		if (isVolume(currentUv, &state) && etype == notVolumn)
		{
			//printf("insert :%d\n", x);
			listIndex = atomicAdd(&d_atomic, 1);
			atomicExch(nextPtr, listIndex);// write listIndex to next slot
			d_listBuffer[listIndex].beginIndex = x;
			d_listBuffer[listIndex].endIndex = x + 1;
			d_listBuffer[listIndex].nextPt = 0;
			nextPtr = (unsigned int *)(&(d_listBuffer[listIndex].nextPt));
			etype = isVolumn;
			
		}
		else if (isVolume(currentUv, &state) && etype == isVolumn)
		{
			
			
		}
		else if (!isVolume(currentUv, &state) && etype == isVolumn)
		{
			//printf("end :%d\n", x);

			d_listBuffer[listIndex].endIndex = x;
			etype = notVolumn;
		}

	}
}
__global__ void renderToTexutre(int kernelWidth, int kernelHeight)
{
	int y = __umul24(blockIdx.y, blockDim.y) + threadIdx.y;
	if (y > kernelHeight)
		return;
	
}
ListNote *device_data = NULL;
int atomBuffer = 0;
#ifdef DEBUG
	ListNote *host_data = NULL;
#endif
extern void cudaInit(int height, int k)
{
	checkCudaErrors(cudaMalloc(&device_data, height*k*sizeof(ListNote)));
	
	checkCudaErrors(cudaMemcpyToSymbol(d_listBuffer, &device_data,sizeof(ListNote*)));
	
	checkCudaErrors(cudaMemcpyToSymbol(d_atomic, &atomBuffer, sizeof(int)));
	checkCudaErrors(cudaMemset(device_data,0, height*k*sizeof(ListNote)));  
#ifdef DEBUG
	checkCudaErrors(cudaMallocHost(&host_data, height*k*sizeof(ListNote)));
#endif
	//host_data = (ListNote*)malloc(height*k*sizeof(ListNote));
	//memset(host_data, 0, height*k*sizeof(ListNote));
	//checkCudaErrors(cudaMemcpy((void *)device_data, (void *)host_data, height * k * sizeof(ListNote), cudaMemcpyDeviceToHost));

	
}
extern "C" void countRow(int width, int height)
{

	cudaEvent_t begin_t, end_t;
	checkCudaErrors(cudaEventCreate(&begin_t));
	checkCudaErrors(cudaEventCreate(&end_t));

	cudaEventRecord(begin_t, 0);

	checkCudaErrors(cudaMemcpyToSymbol(d_atomic, &atomBuffer, sizeof(int)));
	checkCudaErrors(cudaMemset(cuda_PBO_Buffer, 0, height*sizeof(uint4)));

	dim3 blockSize(1, 16, 1);
	dim3 gridSize(1, height / blockSize.y, 1);
	countRowKernel << <gridSize, blockSize >> >(1, height);
	cudaEventRecord(end_t, 0);
	cudaEventSynchronize(end_t);
	float costtime;
	checkCudaErrors(cudaEventElapsedTime(&costtime, begin_t, end_t));

	checkCudaErrors(cudaEventDestroy(begin_t));
	checkCudaErrors(cudaEventDestroy(end_t));

#ifdef DEBUG
	int arraySize = 0;
	checkCudaErrors(cudaMemcpy((void *)host_data, (void *)device_data, height*10*sizeof(ListNote), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpyFromSymbol(&arraySize, d_atomic, sizeof(int)));

	for (int i = 0; i < arraySize; i++)
	{
		printf("b:%d,n:%d,next:%d\n", host_data[i].beginIndex, host_data[i].endIndex, host_data[i].nextPt);
	}
#endif

}

extern "C"  void cudaRelateTex(CudaTexResourse * pResouce)
{

	cudaArray *tmpcudaArray;
	cudaGraphicsResource ** pCudaTex = pResouce->getResPoint();
	checkCudaErrors(cudaGraphicsSubResourceGetMappedArray(&tmpcudaArray, *pCudaTex, 0, 0));
	int w = pResouce->getWidth();
	int h = pResouce->getHeight();
	checkCudaErrors(cudaMemcpyToSymbol(imageWidth, &w, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(imageHeight, &h, sizeof(int)));
	if (occluderbuffer_t == pResouce->getType())
	{
		checkCudaErrors(cudaBindTextureToArray(cudaOccuderTex, tmpcudaArray, channelDesc));
		cudaOccuderTex.filterMode = cudaFilterModeLinear;
	}
	else if (edgebuffer_t == pResouce->getType())
	{
		checkCudaErrors(cudaBindTextureToArray(cudaEdgeTex, tmpcudaArray, channelDesc));
		cudaEdgeTex.filterMode = cudaFilterModePoint;
	}
	else if (color_t == pResouce->getType())
	{
		checkCudaErrors(cudaBindTextureToArray(cudaColorTex, tmpcudaArray, channelDesc));
		cudaColorTex.filterMode = cudaFilterModePoint;
	}

}
extern "C" void cudaRelateArray(CudaPboResource * pResource)
{
	size_t numBytes;
	cudaGraphicsResource ** pCudaTex = pResource->getResPoint();
	int w = pResource->getWidth();
	int h = pResource->getHeight();
	if (unit_4 == pResource->getType())
	{
		checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void**)&cuda_PBO_Buffer, &numBytes, *pCudaTex));
		checkCudaErrors(cudaMemcpyToSymbol(d_cudaPboBuffer, &cuda_PBO_Buffer, sizeof(uint4*)));
	}
	else if (float4_t == pResource->getType())
	{
		checkCudaErrors(cudaMemcpyToSymbol(d_outTextureWidth,  &w, sizeof(int)));
		checkCudaErrors(cudaMemcpyToSymbol(d_outTextureHeigh, &h, sizeof(int)));

		checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void**)&cuda_TexturePbo_buffer, &numBytes, *pCudaTex));
		checkCudaErrors(cudaMemcpyToSymbol(d_cudaTexture, &cuda_TexturePbo_buffer, sizeof(float4*)));
	}
}
