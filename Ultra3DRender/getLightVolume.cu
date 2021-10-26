#ifndef _GETLIGHTVOLUME_CU
#define _GETLIGHTVOLUME_CU

#include <helper_cuda.h>
#include <curand_kernel.h>
#include <cuda_runtime.h>

#include "cu_tool.h"
////////////////////////////////////////////////////////////
//render the volume
__global__ void
kernel_calDirect_x(float* pbo,unsigned int x,unsigned int lastx,float*c)
{
	unsigned int y = threadIdx.x;//光照纹理的y
	unsigned int z = blockIdx.x;//光照纹理的z
	unsigned int w =  size[0] - 1;//光照纹理的大小 w，h，d。
	unsigned int h =  size[1] - 1;
	unsigned int d =  size[2] - 1;
	unsigned int i =  x + y * (w + 1) + z * (h + 1) * (w + 1);//指这个光照三维体素中第几个体素
	float3 ObjectCoor = make_float3(x*1.0/w,y*1.0/h,z*1.0/d);//进入到该核函数的两层光照纹理体素的每一个体素的坐标（相对于体素块的原点（左下角）归一化处理后的位置）

	

	ObjectCoor = toObjectWorldCoor(ObjectCoor);//在具体需要渲染的体素中的坐标（相对于所要渲染的原点）
	float3 lightCoor = make_float3(cudaLightPositionAfter[0],cudaLightPositionAfter[1],cudaLightPositionAfter[2]);//光照的位置，对于每一个体素，光照的坐标保持不变，相对于渲染体素的位置
	//set the light style
	//float3 ObjectToLightV =  normalize(lightCoor - ObjectCoor);
	float3 ObjectToLightV =  normalize(lightCoor);//归一化后的光照位置向量
	float cosine = int(lastx - x)*ObjectToLightV.x;//光照的x坐标。
	unsigned int target_y =  toTexWorldCoorAxis(ObjectToLightV.y /cosine/w  + ObjectCoor.y) * h ;//转化为相对于光照纹理的位置
	unsigned int target_z =  toTexWorldCoorAxis(ObjectToLightV.z /cosine/w  + ObjectCoor.z) * d ;//对应比例，虽然两个立方体大小不一致，但是求的是对应位置的像素值。
	float intensity = tex3D(tex,lastx*1.0f/w,target_y*1.0f/h,target_z*1.0f/d)* correction;//要渲染体素块目标位置像素的像素值。
    float alpha = tex1D(tftex,intensity);//这个强度像素值对应的α大小（这个强度值对应的tf数据中的数）（透明度）
	
	
	float4 data;
	surf2Dread(&data,lightSurface, target_y * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);//将lightSurface内的数据读入data
    ///////////////////
	float4 dataTemp;
	float4 dataSum = make_float4(0,0,0,0);
	float alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,(target_y - 1)*1.0f/h,target_z*1.0f/d) * correction);//y方向-1位置的像素值所对应的α大小
	surf2Dread(&dataTemp,lightSurface,(target_y - 2) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;//渲染体素块y-1处像素值强度对应的tf数值(透明度)，用该透明度*(y-2)处对应的surface2数值，来作为y处的surface的数值。
	//并且将此数值也作为光照3D纹理y*4处依次4位的数值。该数值代表对应像素点的光照强度。
  

	alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,(target_y + 1)*1.0f/h,target_z*1.0f/d)* correction);//y+1
	surf2Dread(&dataTemp,lightSurface,(target_y + 2) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;
  

	alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,(target_y )*1.0f/h,(target_z-1)*1.0f/d)* correction);//z-1
	surf2Dread(&dataTemp,lightSurface,(target_y) * SURFACE_ALIGN_BYTES, target_z - 2,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,(target_y )*1.0f/h,(target_z+1)*1.0f/d)* correction);//z+1
	surf2Dread(&dataTemp,lightSurface,(target_y) * SURFACE_ALIGN_BYTES, (target_z + 2),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

    dataSum*=0.25f;//取均值


   	data.x = dataSum.x ;
	data.y = dataSum.y ;
	data.z = dataSum.z ;
    //////////////////////////////
	data.w = (1.0f - alpha)*data.w;
	surf2Dwrite(data,lightSurface2,target_y * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);//将data内的数据写入lightSurface2。
	
	float weight = 1.0f - 2.0f * acosf(cosine)/CUDART_PI_F;
	pbo[i*4 + 3] += data.w * weight;
	pbo[i*4 + 0] += data.x * weight;
	pbo[i*4 + 1] += data.y * weight;
	pbo[i*4 + 2] += data.z * weight;

}
__global__ void
kernel_calDirect_swap_x(float* pbo,unsigned int x,unsigned int lastx,float*c)
{

	unsigned int y = threadIdx.x;
	unsigned int z = blockIdx.x;
	unsigned int w =  size[0] - 1;
	unsigned int h =  size[1] - 1;
	unsigned int d =  size[2] - 1;
	unsigned int i =  x + y * (w + 1) + z * (h + 1) * (w + 1);
	float3 ObjectCoor = make_float3(x*1.0/w,y*1.0/h,z*1.0/d);

	
	
	ObjectCoor = toObjectWorldCoor( ObjectCoor);
	float3 lightCoor = make_float3(cudaLightPositionAfter[0],cudaLightPositionAfter[1],cudaLightPositionAfter[2]);
	//set the light style
	//float3 ObjectToLightV =  normalize(lightCoor - ObjectCoor);
	float3 ObjectToLightV =  normalize(lightCoor);
	float cosine = int(lastx - x)*ObjectToLightV.x;
	unsigned int target_y =  toTexWorldCoorAxis(ObjectToLightV.y /cosine/w  + ObjectCoor.y) * h ;
	unsigned int target_z =  toTexWorldCoorAxis(ObjectToLightV.z /cosine/w  + ObjectCoor.z) * d ;
   
	float intensity = tex3D(tex,lastx*1.0f/w,target_y*1.0f/h,target_z*1.0f/d)* correction;
    float alpha = tex1D(tftex,intensity);
	
	
	float4 data;
	surf2Dread(&data,lightSurface2, target_y * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
    ////////////////////////////////
	float4 dataTemp;
	float4 dataSum = make_float4(0,0,0,0);
	float alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,(target_y -1)*1.0f/h,target_z*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2,(target_y -1) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,target_y*1.0f/h,(target_z - 1)*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2,target_y * SURFACE_ALIGN_BYTES, (target_z - 1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,(target_y+1)*1.0f/h,target_z*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2,(target_y+ 1) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,lastx*1.0f/w,target_y*1.0f/h,(target_z + 1)*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2,target_y * SURFACE_ALIGN_BYTES, (target_z + 1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	dataSum*=0.25f;

  	data.x = dataSum.x ;
	data.y = dataSum.y ;
	data.z = dataSum.z ;
    //////////////////////////////////
	data.w = (1.0f - alpha)*data.w;
	surf2Dwrite(data,lightSurface,target_y * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	
	float weight = 1.0f - 2.0f * acosf(cosine)/CUDART_PI_F;
	pbo[i*4 + 3] += data.w * weight;
	pbo[i*4 + 0] += data.x * weight;
	pbo[i*4 + 1] += data.y * weight;
	pbo[i*4 + 2] += data.z * weight;


}
///////////////////////////////////////////////////////////////
__global__ void
kernel_calDirect_y(float* pbo,unsigned int y,unsigned int lasty,float*c)
{
	
	unsigned int x = threadIdx.x;
	unsigned int z = blockIdx.x;
	unsigned int w =  size[0] - 1;
	unsigned int h =  size[1] - 1;
	unsigned int d =  size[2] - 1;
	unsigned int i =  x + y * (w + 1) + z * (h + 1) * (w + 1);
	float3 ObjectCoor = make_float3(x*1.0/w,y*1.0/h,z*1.0/d);

	
	
	ObjectCoor = toObjectWorldCoor(ObjectCoor);

	float3 lightCoor = make_float3(cudaLightPositionAfter[0],cudaLightPositionAfter[1],cudaLightPositionAfter[2]);
	//float3 ObjectToLightV =  normalize(lightCoor - ObjectCoor);
	float3 ObjectToLightV =  normalize(lightCoor);
	float cosine = int(lasty - y)*ObjectToLightV.y;
	unsigned int target_x =   toTexWorldCoorAxis(ObjectToLightV.x /cosine/h + ObjectCoor.x ) * w ;
	unsigned int target_z =   toTexWorldCoorAxis(ObjectToLightV.z /cosine/h  + ObjectCoor.z ) * d ;
  
	float intensity = tex3D(tex,target_x*1.0f/w,lasty*1.0f/h,target_z*1.0f/d)* correction;
    float alpha = tex1D(tftex,intensity);
	
	float4 data;
	surf2Dread(&data,lightSurface, target_x * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	///////////////////
	float4 dataTemp;
	float4 dataSum = make_float4(0,0,0,0);
	float alphaTemp = tex1D(tftex,tex3D(tex,(target_x-1)*1.0f/w,lasty*1.0f/h,target_z*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface, (target_x-1) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,lasty*1.0f/h,(target_z-1)*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface, target_x * SURFACE_ALIGN_BYTES, target_z-1,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,(target_x + 1)*1.0f/w,lasty*1.0f/h,target_z*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface, (target_x+ 1) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,lasty*1.0f/h,(target_z+ 1)*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface, target_x * SURFACE_ALIGN_BYTES, (target_z + 1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

    dataSum*=0.25f;

   	data.x = dataSum.x ;
	data.y = dataSum.y ;
	data.z = dataSum.z ;
    //////////////////////////////
    data.w = (1.0f - alpha)*data.w;
	surf2Dwrite(data,lightSurface2,target_x * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);

    float weight = 1.0f - 2.0f * acosf(cosine)/CUDART_PI_F;
	pbo[i*4 + 3] += data.w * weight;
	pbo[i*4 + 0] += data.x * weight;
	pbo[i*4 + 1] += data.y * weight;
	pbo[i*4 + 2] += data.z * weight;


	
}
__global__ void
kernel_calDirect_swap_y(float* pbo,unsigned int y,unsigned int lasty,float*c)
{
	
	unsigned int x = threadIdx.x;
	unsigned int z = blockIdx.x;
	unsigned int w =  size[0] - 1;
	unsigned int h =  size[1] - 1;
	unsigned int d =  size[2] - 1;
	unsigned int i =  x + y * (w + 1) + z * (h + 1) * (w + 1);
	float3 ObjectCoor = make_float3(x*1.0/w,y*1.0/h,z*1.0/d);

	
	ObjectCoor = toObjectWorldCoor(ObjectCoor);
	
	float3 lightCoor = make_float3(cudaLightPositionAfter[0],cudaLightPositionAfter[1],cudaLightPositionAfter[2]);
	//float3 ObjectToLightV =  normalize(lightCoor - ObjectCoor);
	float3 ObjectToLightV =  normalize(lightCoor);
	float cosine = int(lasty - y)*ObjectToLightV.y;
	unsigned int target_x =   toTexWorldCoorAxis(ObjectToLightV.x /cosine/h + ObjectCoor.x ) * w ;
	unsigned int target_z =   toTexWorldCoorAxis(ObjectToLightV.z /cosine/h  + ObjectCoor.z ) * d ;
    
	float intensity = tex3D(tex,target_x*1.0f/w,lasty*1.0f/h,target_z*1.0f/d)* correction;
    float alpha = tex1D(tftex,intensity);

	float4 data;
	surf2Dread(&data,lightSurface2, target_x * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	///////////////////
	float4 dataTemp;
	float4 dataSum = make_float4(0,0,0,0);
	float alphaTemp = tex1D(tftex,tex3D(tex,(target_x-1)*1.0f/w,lasty*1.0f/h,target_z*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2, (target_x-1) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,lasty*1.0f/h,(target_z-1)*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2, target_x * SURFACE_ALIGN_BYTES, target_z-1,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,(target_x + 1)*1.0f/w,lasty*1.0f/h,target_z*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2, (target_x+ 1) * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,lasty*1.0f/h,(target_z+ 1)*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2, target_x * SURFACE_ALIGN_BYTES, (target_z + 1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;
	dataSum*=0.25f;

    data.x = dataSum.x ;
	data.y = dataSum.y ;
	data.z = dataSum.z ;
    //////////////////////////////
    data.w = (1.0f - alpha)*data.w;
	surf2Dwrite(data,lightSurface,target_x * SURFACE_ALIGN_BYTES, target_z,cudaBoundaryModeClamp);

	float weight = 1.0f - 2.0f * acosf(cosine)/CUDART_PI_F;
	pbo[i*4 + 3] += data.w * weight;
	pbo[i*4 + 0] += data.x * weight;
	pbo[i*4 + 1] += data.y * weight;
	pbo[i*4 + 2] += data.z * weight;


	
}
////////////////////////////////////////////////////////////////
__global__ void
kernel_calDirect_z(float* pbo,unsigned int z,unsigned int lastz,float*c)
{

	unsigned int x = threadIdx.x ;
	unsigned int y = blockIdx.x ;
	unsigned int w =  size[0] - 1;
	unsigned int h =  size[1] - 1;
	unsigned int d =  size[2] - 1 ;
	unsigned int i =  x + y * (w + 1) + z * (h + 1) * (w + 1);
	float3 ObjectCoor = make_float3(x*1.0/w,y*1.0/h,z*1.0/d);
	
	ObjectCoor = toObjectWorldCoor(ObjectCoor);

	

   
	float3 lightCoor = make_float3(cudaLightPositionAfter[0],cudaLightPositionAfter[1],cudaLightPositionAfter[2]);
	//float3 ObjectToLightV =  normalize(lightCoor - ObjectCoor);
	float3 ObjectToLightV =  normalize(lightCoor);

	float cosine = int(lastz - z)*ObjectToLightV.z;
	unsigned int target_y =  toTexWorldCoorAxis(ObjectToLightV.y /cosine/d  + ObjectCoor.y ) * h ;
	unsigned int target_x =  toTexWorldCoorAxis(ObjectToLightV.x /cosine/d + ObjectCoor.x ) * w;
   
	float intensity = tex3D(tex,target_x*1.0f/w,target_y*1.0f/h,lastz*1.0f/d)* correction;
    float alpha = tex1D(tftex,intensity);
	
	
	float4 data;
	surf2Dread(&data,lightSurface, target_x * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);
	///////////////////
	float4 dataTemp;
	float4 dataSum = make_float4(0,0,0,0);
	float alphaTemp = tex1D(tftex,tex3D(tex,(target_x-1)*1.0f/w,target_y*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface, (target_x-1) * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,(target_y-1)*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface,target_x * SURFACE_ALIGN_BYTES, (target_y-1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,(target_x+1)*1.0f/w,target_y*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface,(target_x+1) * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,(target_y+1)*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface, target_x * SURFACE_ALIGN_BYTES, (target_y-1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

    dataSum*=0.25f;

    data.x = dataSum.x ;
	data.y = dataSum.y ;
	data.z = dataSum.z ;
    //////////////////////////////
	data.w = (1.0f - alpha)*data.w;
	surf2Dwrite(data,lightSurface2,target_x * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);

    float weight = 1.0f - 2.0f * acosf(cosine)/CUDART_PI_F;
	pbo[i*4 + 3] += data.w * weight;
	pbo[i*4 + 0] += data.x * weight;
	pbo[i*4 + 1] += data.y * weight;
	pbo[i*4 + 2] += data.z * weight;


	
  
	
}
__global__ void
kernel_calDirect_swap_z(float* pbo,unsigned int z,unsigned int lastz,float*c)
{

	unsigned int x = threadIdx.x ;
	unsigned int y = blockIdx.x ;
	unsigned int w =  size[0] - 1;
	unsigned int h =  size[1] - 1;
	unsigned int d =  size[2] - 1 ;
	unsigned int i =  x + y * (w + 1) + z * (h + 1) * (w + 1);
	float3 ObjectCoor = make_float3(x*1.0/w,y*1.0/h,z*1.0/d);
	
	ObjectCoor = toObjectWorldCoor(ObjectCoor);


   
	float3 lightCoor = make_float3(cudaLightPositionAfter[0],cudaLightPositionAfter[1],cudaLightPositionAfter[2]);
	//float3 ObjectToLightV =  normalize(lightCoor - ObjectCoor);
	float3 ObjectToLightV =  normalize(lightCoor);
	float cosine = int(lastz - z)*ObjectToLightV.z;
	unsigned int target_y =  toTexWorldCoorAxis(ObjectToLightV.y /cosine/d  + ObjectCoor.y ) * h ;
	unsigned int target_x =  toTexWorldCoorAxis(ObjectToLightV.x /cosine/d + ObjectCoor.x ) * w;

	float intensity =  tex3D(tex,target_x*1.0f/w,target_y*1.0f/h,lastz*1.0f/d)* correction;
    float alpha = tex1D(tftex,intensity);
	

	float4 data;
	surf2Dread(&data,lightSurface2, target_x * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);
	///////////////////
	float4 dataTemp;
	float4 dataSum = make_float4(0,0,0,0);
	float alphaTemp = tex1D(tftex,tex3D(tex,(target_x-1)*1.0f/w,target_y*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2, (target_x-1) * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;
	
	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,(target_y-1)*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2,target_x * SURFACE_ALIGN_BYTES, (target_y-1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,(target_x+1)*1.0f/w,target_y*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2,(target_x+1) * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;

	alphaTemp = tex1D(tftex,tex3D(tex,target_x*1.0f/w,(target_y+1)*1.0f/h,lastz*1.0f/d)* correction);
	surf2Dread(&dataTemp,lightSurface2, target_x * SURFACE_ALIGN_BYTES, (target_y+1),cudaBoundaryModeClamp);
	dataSum += (1.0 - alphaTemp) * dataTemp;
	dataSum*=0.25f;


  	data.x = dataSum.x ;
	data.y = dataSum.y ;
	data.z = dataSum.z ;
    //////////////////////////////
	data.w = (1.0f - alpha)*data.w;
	surf2Dwrite(data,lightSurface,target_x * SURFACE_ALIGN_BYTES, target_y,cudaBoundaryModeClamp);
	
	float weight = 1.0f - 2.0f * acosf(cosine)/CUDART_PI_F;
	pbo[i*4 + 3] += data.w * weight;
	pbo[i*4 + 0] += data.x * weight;
	pbo[i*4 + 1] += data.y * weight;
	pbo[i*4 + 2] += data.z * weight;


	
}
//////////////////////////////////////////render gbobal illumination volume //////////////////////////////
void render_Kernel_GI(dim3 grid,dim3 block,float* pbo,float *lightPosition,int * surfaceSize,float*c)
{
	//initialize light volume
	kernel_initial_light_volume << <grid, block >> > (pbo, c);
	checkCudaErrors(cudaDeviceSynchronize());
    dim3 surfaceGrid(surfaceSize[1]);
    dim3 surfaceBlock(surfaceSize[0]);
	
	int w = block.x;//光照3D纹理的长宽高
	int h = grid.x;
	int d = grid.y;

	dim3 x(w);
    dim3 y(h);
	dim3 z(d);
	float _lightPosition[3] = {0.0};
	if(lightPosition[0] < 0.0)
	{
		_lightPosition[0] = - lightPosition[0];
		
	}
	if(lightPosition[0] > 0.0)
	{
		_lightPosition[0] =  lightPosition[0];
		
	}
	if(lightPosition[1] > 0.0)
	{
		_lightPosition[1] =  lightPosition[1];
		
	}
	if(lightPosition[1] < 0.0)
	{
		_lightPosition[1] = - lightPosition[1];
		
	}
	if(lightPosition[2] < 0.0)
	{
		_lightPosition[2] = - lightPosition[2];
		
	}
	if(lightPosition[2] > 0.0)
	{
		_lightPosition[2] =  lightPosition[2];
		
	}

    checkCudaErrors(cudaDeviceSynchronize());

	//frame of rendering /////////////////////////////////////////
	for(int times = 0;times < 2;times++)
	{
		//initialize the buffer 
		kernel_initial_light_surface<<<surfaceGrid,surfaceBlock>>>();
		checkCudaErrors(cudaDeviceSynchronize());

		if(_lightPosition[0] >= _lightPosition[1] && _lightPosition[0] > _lightPosition[2])
		{
			if(lightPosition[0] > 0)
			{
				for(int i = w - 1;i >= 0;i--)
				{ 
					if(i%2 == 0)
					{
						kernel_calDirect_x<<<z,y>>>(pbo,i,i + 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
					else
					{
						kernel_calDirect_swap_x<<<z,y>>>(pbo,i,i + 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
				
				}

			}else
			{
				for(int i = 0;i < w;i++)
				{
					if(i%2 == 0)
					{
						kernel_calDirect_x<<<z,y>>>(pbo,i,i - 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
					else
					{
						kernel_calDirect_swap_x<<<z,y>>>(pbo,i,i - 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
				}		
			
			}
			_lightPosition[0] = 0;

		}
		else if(_lightPosition[1] > _lightPosition[0] && _lightPosition[1] > _lightPosition[2])
		{
			if(lightPosition[1] > 0)
			{
				for(int i = h - 1;i >= 0;i--)
				{
					if(i%2 == 0)
					{
						kernel_calDirect_y<<<z,x>>>(pbo,i,i + 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
					else
					{
						kernel_calDirect_swap_y<<<z,x>>>(pbo,i,i + 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}	
				}
	
			}else
			{
				for(int i = 0;i < h ;i++)
				{
					if(i%2 == 0)
					{
						kernel_calDirect_y<<<z,x>>>(pbo,i,i - 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
					else
					{
						kernel_calDirect_swap_y<<<z,x>>>(pbo,i,i - 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
				
				}
			
			}
			_lightPosition[1] = 0;

		}
		else
		{
			if(lightPosition[2] > 0)
			{
				for(int i = d - 1;i >= 0;i--)
				{
					if(i%2 == 0)
					{
						kernel_calDirect_z << <y, x >> > (pbo, i, i + 1, c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
					else
					{
						kernel_calDirect_swap_z<<<y,x>>>(pbo,i,i + 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}		
				
				}			
						
			}
			else
			{
				for(int i = 0;i < d ;i++)
				{
					if(i%2 == 0)
					{
						kernel_calDirect_z<<<y,x>>>(pbo,i,i - 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
					else
					{
						kernel_calDirect_swap_z<<<y,x>>>(pbo,i,i - 1,c);
						checkCudaErrors(cudaDeviceSynchronize());
					}
				
				}
			
			}
			_lightPosition[2] = 0;

		}
    }

	

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//initialize the data of volume
cudaArray* InitialCudaData(void*data,const unsigned int &w,const unsigned int &h,const unsigned int &d, const unsigned int &dataType,float _correction)
{
	cudaArray *cuArray;
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(dataType * 8,0,0,0,cudaChannelFormatKindUnsigned);
	cudaExtent extent;
	extent.width = w;
	extent.height = h;
	extent.depth = d;
	checkCudaErrors(cudaMalloc3DArray(&cuArray,&channelDesc,extent));
    
	cudaMemcpy3DParms copyParams = {0};
	copyParams.srcPtr = make_cudaPitchedPtr((void*)data,w * dataType,w,h);
	copyParams.dstArray = cuArray;
	copyParams.extent = extent;
	copyParams.kind = cudaMemcpyHostToDevice;
    cudaMemcpy3D(&copyParams);
    //texture with float coordinate,with Liner filter;
	tex.filterMode = cudaFilterModeLinear;
	tex.normalized = true;
	tex.addressMode[0] = cudaAddressModeClamp;
	tex.addressMode[1] = cudaAddressModeClamp;
	tex.addressMode[2] = cudaAddressModeClamp;
	tex.channelDesc = channelDesc;
	checkCudaErrors(cudaBindTextureToArray(tex,cuArray,channelDesc));
	//because the data don't full of size of format
	checkCudaErrors(cudaMemcpyToSymbol(correction,&_correction,sizeof(float)));

	return cuArray;

}

//initialize the data of transform function
cudaArray* InitialCudaTranData(void* tfdata)
{
	cudaArray *cuArray;
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(8,0,0,0,cudaChannelFormatKindUnsigned);
	checkCudaErrors(cudaMallocArray(&cuArray,&channelDesc,256,1));

	checkCudaErrors(cudaMemcpyToArray(cuArray,0,0,tfdata,256,cudaMemcpyHostToDevice));

	tftex.filterMode = cudaFilterModeLinear;
	tftex.normalized = true;
	tftex.addressMode[0] = cudaAddressModeClamp;
	tftex.channelDesc = channelDesc;
	checkCudaErrors(cudaBindTextureToArray(tftex,cuArray,channelDesc));

	return cuArray;

}
//initialize the data of scattering coffi
//cudaArray*  InitialScatteringData(void* scatteringData)
//{
//	cudaArray *cuArray;
//	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(8,0,0,0,cudaChannelFormatKindUnsigned);
//	checkCudaErrors(cudaMallocArray(&cuArray,&channelDesc,256,1));
//
//	checkCudaErrors(cudaMemcpyToArray(cuArray,0,0,scatteringData,256,cudaMemcpyHostToDevice));
//
//	tftex.filterMode = cudaFilterModeLinear;
//	tftex.normalized = true;
//	tftex.addressMode[0] = cudaAddressModeClamp;
//	tftex.channelDesc = channelDesc;
//	checkCudaErrors(cudaBindTextureToArray(scattertex,cuArray,channelDesc));
//
//	return cuArray;
//
//}
//set the other parameters
void setSizeParameters(int w,int h,int d)
{
	int _size[3];
	_size[0] = w;
	_size[1] = h;
	_size[2] = d;
	checkCudaErrors(cudaMemcpyToSymbol(size,_size,sizeof(_size)));

}
void setDataSizeParameters(int w,int h,int d)
{
	int _size[3];
	_size[0] = w;
	_size[1] = h;
	_size[2] = d;
	checkCudaErrors(cudaMemcpyToSymbol(dataSize,_size,sizeof(_size)));

}
//set the light position after transform,scaling matrix
void setLightPositionAfter(float* position)
{
	checkCudaErrors(cudaMemcpyToSymbol(cudaLightPositionAfter,position,sizeof(float)*3));
}

//initial light surface
void initialLightSurface(int width,int height,int bits,cudaArray * surfaceArray,cudaArray * surfaceArray2)
{
	//lightSurface
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(bits,bits,bits,bits,cudaChannelFormatKindFloat);
	cudaMallocArray(&surfaceArray,&channelDesc,width,height,cudaArraySurfaceLoadStore);
	cudaBindSurfaceToArray(lightSurface,surfaceArray);
	//lightSurface2 for swap
	cudaMallocArray(&surfaceArray2,&channelDesc,width,height,cudaArraySurfaceLoadStore);
	cudaBindSurfaceToArray(lightSurface2,surfaceArray2);

	int size[2];
	size[0] = width;
	size[1] = height;
    checkCudaErrors(cudaMemcpyToSymbol(lightSurfaceSize,size,sizeof(int)*2));
}


#endif

