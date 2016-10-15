#include "cuda.h"

texture<float4, 2, cudaReadModeElementType> cudaEdgeTex;
texture<float4, 2, cudaReadModeElementType> cudaOccuderTex;
texture<float4, 2, cudaReadModeElementType> cudaColorTex;
cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 32, 32, 32, cudaChannelFormatKindFloat);

typedef enum {
	isVolumn,
	notVolumn,
}pixelEnum;
class ListNote
{
public:

	unsigned int nextPt,beginIndex, endIndex;
};

uint3 *cuda_PBO_Buffer;

__device__ uint3* d_cudaPboBuffer;
float4 *cuda_TexturePbo_buffer;
__device__ float4* d_cudaTexture;
__device__ int imageWidth, imageHeight, d_outTextureWidth, d_outTextureHeigh;
__device__ int d_index;
__device__ ListNote* d_listBuffer;
__device__ int d_atomic;

class List
{
	
};

__device__ bool isVolume(float2 uv, int *state)
{
	//if (uv.x > 230 && uv.x < 300)
	//	return true;
	float4 value = tex2D(cudaOccuderTex, uv.x, uv.y);
	return value.x > 0.5;
}
__device__ float2 toUv(int x, int y)
{
	return make_float2(x + 0.5, y + 0.5);
}
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
		float2 currentUv = toUv(x, y);
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
__device__ void FillVolumn(int beginX, int endX, int y)
{
	int top = min(endX, d_outTextureWidth);
	//printf("volumn begin:%d,end:%d,top:%d\n",beginX,endX,top);
	for (int x = beginX; x < top; x++)
	{
		int index = y*d_outTextureWidth + x;
		d_cudaTexture[index] =make_float4(1,0,0,1);
		//printf("fillPixel(%d,%d),index%d  (%f,%f)\t", x, y, index, uvx, beginUv.y);

	}
}
__device__ void FillSpan(int beginX, int endX, int y,float2 beginUv,float2 endUv)
{
	int top = min(endX, d_outTextureWidth);
	//printf("tx: begin:%d,end:%d,top:%d,(%f,%f)\n",beginX,endX,top,beginUv.x,endUv.x);
	for (int x = beginX; x < top; x++)
	{
		int index = y*d_outTextureWidth+x;
		float uvx = beginUv.x + (endUv.x - beginUv.x) / (top - 1)*x;
		d_cudaTexture[index] = tex2D(cudaColorTex, uvx, beginUv.y);
		//printf("fillPixel(%d,%d),index%d  (%f,%f)\t", x, y, index, uvx, beginUv.y);

	}
}
__global__ void renderToTexutre(int kernelWidth, int kernelHeight)
{
	int y = __umul24(blockIdx.y, blockDim.y) + threadIdx.y;
	if (y > kernelHeight)
		return;
	int listIndex = y;
	int rowLength = imageWidth;
	ListNote currentNote =* ((ListNote*)&d_cudaPboBuffer[listIndex]);
	//if (y != 599)
	//	return;
	int texEnd =0;
	int texBegin = 0;
	int fillBegin = 0;
	int fillEnd = 0;
	int acuumPixel =0,span =0;
	//("begin:%d,end%d,index:%d\n", currentNote.beginIndex, currentNote.endIndex, currentNote.nextPt);
	//printf("init:%d\n", d_cudaPboBuffer[listIndex].x);
	while (currentNote.nextPt != 0)
	{
		currentNote = d_listBuffer[currentNote.nextPt];
		texEnd = currentNote.endIndex;
		span = currentNote.endIndex - currentNote.beginIndex;
		fillBegin = texBegin + acuumPixel;
		fillEnd = texEnd + acuumPixel;
		FillSpan(fillBegin, fillEnd, y, toUv(texBegin, y), toUv(texEnd, y));
		FillVolumn(fillEnd, fillEnd+span, y);
		
		acuumPixel += span;
		texBegin = currentNote.endIndex;
		
	}
	fillBegin = texBegin + acuumPixel;
	//printf("final:(%d,%d) u(%f,%f)\n", fillBegin, imageWidth + span, toUv(texBegin, y).x, toUv(imageWidth - 1, y).x);

	FillSpan(fillBegin, imageWidth + span, y, toUv(texBegin, y), toUv(imageWidth - 1, y));

	
}
ListNote *device_data = NULL;
int atomBuffer = 0;
#ifdef DEBUG
	ListNote *host_data = NULL;
#endif
extern void cudaInit(int height, int width,int k,int rowLarger)
{
	checkCudaErrors(cudaMalloc(&device_data, height*k*sizeof(ListNote)));
	
	checkCudaErrors(cudaMemcpyToSymbol(d_listBuffer, &device_data,sizeof(ListNote*)));
	
	checkCudaErrors(cudaMemcpyToSymbol(d_atomic, &atomBuffer, sizeof(int)));
	checkCudaErrors(cudaMemset(device_data,0, height*k*sizeof(ListNote)));  
	//checkCudaErrors(cudaMemset(cuda_TexturePbo_buffer, 0, width* height*rowLarger*sizeof(float4)));
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
	checkCudaErrors(cudaMemset(cuda_PBO_Buffer, 0, height*sizeof(uint3)));
	checkCudaErrors(cudaMemset(cuda_TexturePbo_buffer, 0, ROWLARGER*width*height*sizeof(float4)));

	dim3 blockSize(1, 16, 1);
	dim3 gridSize(1, height / blockSize.y, 1);
	countRowKernel << <gridSize, blockSize >> >(1, height);
	cudaEventRecord(end_t, 0);
	cudaEventSynchronize(end_t);
	float costtime;
	checkCudaErrors(cudaEventElapsedTime(&costtime, begin_t, end_t));

	renderToTexutre << <gridSize, blockSize >> >(1, height);

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
	if (unit_3 == pResource->getType())
	{
		checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void**)&cuda_PBO_Buffer, &numBytes, *pCudaTex));
		checkCudaErrors(cudaMemcpyToSymbol(d_cudaPboBuffer, &cuda_PBO_Buffer, sizeof(uint3*)));
	}
	else if (float4_t == pResource->getType())
	{
		checkCudaErrors(cudaMemcpyToSymbol(d_outTextureWidth,  &w, sizeof(int)));
		checkCudaErrors(cudaMemcpyToSymbol(d_outTextureHeigh, &h, sizeof(int)));

		checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void**)&cuda_TexturePbo_buffer, &numBytes, *pCudaTex));
		checkCudaErrors(cudaMemcpyToSymbol(d_cudaTexture, &cuda_TexturePbo_buffer, sizeof(float4*)));
	}
}
