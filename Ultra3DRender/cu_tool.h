#ifndef CU_TOOL_H
#define CU_TOOL_H

#ifndef __CUDACC__
#define __CUDACC__
#include "cuda_texture_types.h"
#include <cuda_surface_types.h>
#include <surface_functions.h>
#include <device_launch_parameters.h>
#include <texture_fetch_functions.h>
#include <cuda_gl_interop.h>
#endif
#include <math_constants.h>
#include <helper_math.h>
//texture format
texture<unsigned char,cudaTextureType3D,cudaReadModeNormalizedFloat> tex;
texture<unsigned char,cudaTextureType1D,cudaReadModeNormalizedFloat> tftex;
texture<unsigned char,cudaTextureType1D,cudaReadModeNormalizedFloat> scattertex;

surface<void,cudaSurfaceType2D> lightSurface;
surface<void,cudaSurfaceType2D> lightSurface2;
//contain w h d illumination size
__constant__ int size[3];
//datasize
__constant__ int dataSize[3];
//__constant__ float step;
__constant__ float cudaLightPositionAfter[3];
//the project buffer with and height
__constant__ int lightSurfaceSize[2];
//data correction
__constant__ float correction;
//shape Factor
//__constant__ float shapeFactor;

#define SURFACE_ALIGN_BYTES 16

inline __device__
float3 toObjectWorldCoor(float3 t)
{
	return (t * 2.0f - 1.0f);

}
inline __device__
float toTexWorldCoorAxis(float t)
{
	return (t + 1.0f)/2.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
//initialize the light volume
__global__ void
kernel_initial_light_volume(float *pbo,float *c)
{
	unsigned int x = threadIdx.x;
	unsigned int y = blockIdx.x;
	unsigned int z = blockIdx.y;
	unsigned int i = x + y * size[0] + z * size[0] * size[1];
	pbo[i*4 + 0] = 0;
	pbo[i*4 + 1] = 0;
	pbo[i*4 + 2] = 0;
	pbo[i*4 + 3] = 0;
	
}
//initialize the light surface
__global__ void
kernel_initial_light_surface()
{
    unsigned int x = threadIdx.x;
	unsigned int y = blockIdx.x;
	float4 data = make_float4(1.0f,1.0f,1.0f,1.0f);
	surf2Dwrite(data,lightSurface, x * SURFACE_ALIGN_BYTES, y,cudaBoundaryModeClamp);
	surf2Dwrite(data,lightSurface2, x * SURFACE_ALIGN_BYTES, y,cudaBoundaryModeClamp);
    
}
///////////////////////////////////////////////////////////////
#endif