#include "ultr3d.h"
#include <QtWidgets/qmessagebox.h>
using namespace std;
//CUDA FUNCTION///////////////////////
cudaArray* InitialCudaData(void*data,const unsigned int &w,const unsigned int &h,const unsigned int &d,const unsigned int &dataType,float _correction);
cudaArray* InitialCudaTranData(void* tfdata);
//cudaArray*  InitialScatteringData(void* scatteringData);
void setSizeParameters(int w,int h,int d);
void setDataSizeParameters(int w,int h,int d);
void setLightPositionAfter(float* position);
//initial light surface
void initialLightSurface(int width,int height,int bits,cudaArray * surfaceArray,cudaArray * surfaceArray2);
void render_Kernel_GI(dim3 grid,dim3 block,float* pbo,float *lightPosition,int * surfaceSize,float*c);
//void render_kernel_AOMap(dim3 grid,dim3 block,unsigned char*pbo,float*c);
//////////////////////////////////////
//define const 
const float XMIN = -1.6f;
const float XMAX = 1.6f;
const float YMIN = -1.6f;
const float YMAX = 1.6f;
const float ZMIN = -15.0f;
const float ZMAX = 15.0f;
const int WINDOW_SIZE_HEIGHT = 600;
const int WINDOW_SIZE_WIDTH = 600;
//define the data format
//#define GL_UNSIGNED_BYTE 
//#define GL_UNSIGNED_SHORT 
//#define GL_UNSIGNED_INT 
//#define GL_FLOAT
#define IllVolDataFormat  GL_RGBA32F 

#define SCALE_IllVOLUME_WIDTH  0.5f;
#define SCALE_IllVOLUME_HEIGHT 0.5f;
#define SCALE_IllVOLUME_DEPTH  0.5f;

#define dataFormat GL_UNSIGNED_BYTE
//#define dataFormat GL_UNSIGNED_SHORT

#define SURFACE_BYTES 4 //FLOAT 4; HALF FLOAT 2

ultr3D::ultr3D(QString fileName, QWidget *parent, Qt::WindowFlags flags)
: QGLWidget(parent)
{
	this ->fileName = fileName;
	this ->initializeData();
	
	 //load data
	std::string s = (this->fileName).toUtf8().constData();//.toStdString();
	try
	{
 	    this ->readUltrRawFile(s.c_str());
	}
	catch(V3dException &e)
	{
		QMessageBox::information(NULL, "warning", e.getEception_str(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		qDebug()<< e.getEception_str();

		
	}

	//get gray count//////////////////////////////
	getGrayCount();	
	
	//set the illumination volume size
	this ->IllVolumeWidth = this ->TexW * SCALE_IllVOLUME_WIDTH;
	this ->IllVolumeHeight = this ->TexH * SCALE_IllVOLUME_HEIGHT;
	this ->IllVolumeDepth = this ->TexD * SCALE_IllVOLUME_DEPTH;
	//this ->IllVolumeType = 1;//1 for unsigne char ,2 for ...
	this ->IllVolumeSize = this ->IllVolumeDepth * this ->IllVolumeHeight * this ->IllVolumeWidth;
	this ->IllVolumeSizeOfByte = this ->IllVolumeSize * 4 * this ->IllVolumeType;
}


ultr3D::ultr3D(QString fileName,int width,int height,int depth, QWidget *parent, Qt::WindowFlags flags)
{
	this ->fileName = fileName;
	this ->initializeData();
	//load data
	std::string s = (this ->fileName).toUtf8().constData();//.toStdString();
	try
	{
 	   this ->readCTRawFile(s.c_str(),width,height,depth);
	}
	catch(V3dException &e)
	{
		QMessageBox::information(NULL, "warning", e.getEception_str(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		qDebug()<< e.getEception_str();
		
	}
	

	//get gray count//////////////////////////////
	getGrayCount();	
	
	//set the illumination volume size
	this ->IllVolumeWidth = this ->TexW * SCALE_IllVOLUME_WIDTH;
	this ->IllVolumeHeight = this ->TexH * SCALE_IllVOLUME_HEIGHT;
	this ->IllVolumeDepth = this ->TexD * SCALE_IllVOLUME_DEPTH;
	//this ->IllVolumeType = 1;//1 for unsigne char ,2 for ...
	this ->IllVolumeSize = this ->IllVolumeDepth * this ->IllVolumeHeight * this ->IllVolumeWidth;
	this ->IllVolumeSizeOfByte = this ->IllVolumeSize * 4 * this ->IllVolumeType;
}


ultr3D::~ultr3D()
{	
	this ->deleteData();
}


/*******************************************************/
//initial Data
/********************************************************/
void ultr3D::initializeData()
{
	this ->setEnabled(true);
    this ->setGeometry(QRect(50, 100,WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT));
	//set windows
	QIcon icon;
    icon.addFile(QString::fromUtf8("icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this ->setWindowIcon(icon);
    this ->setFocusPolicy(Qt::ClickFocus);
	 
	//initialize queryFPS
	this ->rotXOld = 0.0f;
	this ->rotYOld = 0.0f;

	this ->leftMouse = false;
	this ->rightMouse = false;

	this ->data = NULL;
	this ->tfData = NULL;
	this ->colorData = NULL;	
	this ->random = NULL;
	this ->TFTex = 0;
	this ->ColorTex = 0;
	this ->RandomTex = 0;
	

	this ->GIllTexPBO = 0;
	this ->GIllTex = 0;


	this ->cuArray = NULL;
	this ->tfArray = NULL;
	this ->scatteringArray = NULL;
	this ->surfaceArray = NULL;
	this ->surfaceArray2 = NULL;


	//Illumination Volume size
	this ->IllVolumeWidth = 0;
	this ->IllVolumeHeight = 0;
	this ->IllVolumeDepth = 0;
	this ->IllVolumeSize = 0;
	this ->IllVolumeSizeOfByte = 0;

	if(IllVolDataFormat ==  GL_RGBA16F)
		this ->IllVolumeType = 2;
	if(IllVolDataFormat == GL_RGBA32F)
	    this ->IllVolumeType = 4;
	//CUDA change which 0 is not change 1 is light change,2 is tf change;
	this ->TFChange = false;
	this ->isLPChange(false);

	//this ->scale = 0.0;
	this ->light = indirectPhongIllum;
	//input data
	this ->dataSize = 0;
	this ->dataSizeOfByte = 0;
	if(dataFormat ==  GL_UNSIGNED_BYTE)
		this ->dataType =  unsignedChar;
	if(dataFormat == GL_UNSIGNED_SHORT)
		this ->dataType = unsignedShort;
	if(dataFormat == GL_UNSIGNED_INT)
		this ->dataType = unsignedInt;

	//the weight of surface shading 
	this ->surfaceShadingParameter = 1.0;
	//this->set1DFunc(0.35f,0.75f);
	this->setSampleStep(800);
	this->setGradientStep(800);


	//HDR on
	this ->hdrOn = true;

	//use tf colors
	this ->useTFColors = true;
	//HDR exposure
	this ->exposure = 2.5f;
	this ->gammaCorrection = 1.6f;
	//light attenuation
	this ->kl = 0;
	//color box
	this ->colorBox = 0.0f;

	//initiaize the light
	m3dLoadVector3(ambient,0.0f,0.0f,0.0f);
	m3dLoadVector3(diffuse,1.0f,1.0f,1.0f);
	m3dLoadVector3(specular,0.0f,0.0f,0.0f);
	m3dLoadVector3(this ->backColor,0.0f,0.0f,0.0f);

	m3dLoadVector4(this ->lightPositionVec4,0.0f,0.0f,1.0f,0.0f);
	m3dScaleVector4(this ->lightPositionVec4,2.5f);

	//phong light
	m3dLoadVector4(this ->lightPositionFixedVec4,0.0f,0.0f,1.0f,0.0f);
	m3dScaleVector4(this ->lightPositionFixedVec4,2.5f);

	m3dLoadVector4(this ->eyePositionVec4,0.0f,0.0f,1.0f,0.0f);

	
	
	//load TF
	this ->tfData = (GLubyte*)malloc(256 * 4);
	//scattering most
	scatteringMost = 1.0f;
}


/*******************************************************/
//delete Data
/********************************************************/
void ultr3D::deleteData()
{

	if(0 != this ->TexObj)
	{
		glDeleteTextures(1,&this ->TexObj);
		this ->TexObj = 0;
	}
	if(0 != this ->ColorTex)
	{
		glDeleteTextures(1,&this ->ColorTex);
		this ->ColorTex = 0;
	}
	
	if(0 != this ->TFTex)
	{
		glDeleteTextures(1,&this ->TFTex);
		this ->TFTex = 0;
	}

	if(0 != this ->GIllTexPBO)
	{
		glDeleteBuffers(1,&this ->GIllTexPBO);
		this ->GIllTexPBO = 0;
	}
	
	if(0 != this ->GIllTex)
	{
		glDeleteTextures(1,&this ->GIllTex);
		this ->GIllTex = 0;
	}
 
    //////////////////////////////
	if(0 != this ->texColorbuffer)
	{
		glDeleteTextures(1,&this ->texColorbuffer);
		this ->texColorbuffer = 0;
	}
	if(0 != this ->ultr)
	{
		glDeleteProgram(this ->ultr);
		this ->ultr = 0;
	}
	if(0 != this ->getBack)
	{
		glDeleteProgram(this ->getBack);
		this ->getBack = 0;
	}
	if(0 != this ->framebuffer)
	{
		glDeleteFramebuffers(1,&this ->framebuffer);
		this ->framebuffer = 0;
	}
	if(0 != this ->rbo)
	{
		glDeleteRenderbuffers(1,&this ->rbo);
		this ->rbo = 0;
	}
	if(0 != this ->queryFPS)
	{
		glDeleteQueries(1,&this ->queryFPS);
		this ->queryFPS = 0;
	}
	if(0 != this ->RandomTex)
	{
		glDeleteTextures(1,&this ->RandomTex);
		this ->RandomTex = 0;
	}

	if(NULL != this ->tfData)
	{
		free(this ->tfData);
		this ->tfData = NULL;

	}
	if(NULL != this ->data)
	{
		free(this ->data);
		this ->data = NULL;

	}
	if(NULL != this ->grayCount)
	{
		free(this ->grayCount);
		this ->grayCount = NULL;
	}

	if(NULL != this ->random)
	{
		free(this ->random);
		this ->random = NULL;
	}
    //FREE CUDA MEMORY AND EXIT
	if(NULL != this ->cuArray)
	{
		checkCudaErrors(cudaFreeArray(this ->cuArray));
		this ->cuArray = NULL;

	}
	if(NULL != this ->tfArray)
	{
		checkCudaErrors(cudaFreeArray(this ->tfArray));
		this ->tfArray = NULL;
	}
	if(NULL != this ->surfaceArray)
	{
		checkCudaErrors(cudaFreeArray(this ->surfaceArray));
		this ->surfaceArray = NULL;
	}
	if(NULL != this ->surfaceArray2)
	{
		checkCudaErrors(cudaFreeArray(this ->surfaceArray2));
		this ->surfaceArray2 = NULL;
	}
	/*if(NULL != this ->scatteringArray)
	{
		checkCudaErrors(cudaFreeArray(this ->scatteringArray));
		this ->scatteringArray = NULL;
	}*/

	checkCudaErrors(cudaDeviceReset());
}


/*******************************************************/
//initializeGL opengl win
/********************************************************/
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void ultr3D::initializeGL()
{
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		qDebug("glewInit error: %s\nglew version: %s\ngl version: %s", glewGetErrorString(err), glewGetString(GLEW_VERSION), glewGetString(GL_VERSION));
		return;
	}
	if (glGetError() != GL_NO_ERROR)
		throw(MemoryException());
	glGenQueries(1,&this ->queryFPS);

	
	//load ultr3d texture
	this ->load3DTex();
	//load Color Texture
	this ->loadColorTex();
	//load tf texture
    this ->loadTFTex();
	//load random texture
	this ->loadRandomTex();
	//load global texture
	try
	{
	    this ->InitGetGlobalIllum();
	}
	catch(V3dException &e)
	{
		QMessageBox::information(NULL, "warning", e.getEception_str(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		qDebug()<< e.getEception_str();
	}
	//load scattering texture
	/*this ->loadScatteringTex();*/
    
	
/////////////////////////////////////////////////////////////////////////////////

	glClearColor(this ->backColor[0],this ->backColor[1],this ->backColor[2],1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	projection.SetOrthographic(XMIN,XMAX,YMIN,YMAX,ZMIN,ZMAX);
	
	projectionStack.LoadMatrix(projection.GetProjectionMatrix());
	//odelviewStack.TranslateToWorld(0.0f,0.0f,-10.0f);
	mvpPipline.SetMatrixStacks(modelviewStack,projectionStack);

	int MaxSize;
	MaxSize = max(TexD, TexH);
	MaxSize = max(MaxSize, TexW);
	CubeW = (float)TexW / MaxSize;
	CubeH = (float)TexH / MaxSize;
	CubeD = (float)TexD / MaxSize;
	makeCube();
    //getBackface///////////////////////////////////
	
	glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); 

    glGenTextures(1,&(this ->texColorbuffer));
	glBindTexture(GL_TEXTURE_2D,texColorbuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,this->width(),this->height(),0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorbuffer, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, this->width(), this->height()); 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	gltCheckErrors();
	glBindFramebuffer(GL_FRAMEBUFFER,0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D,0);
	///////////////////////////////////////////////
    this ->getBack = gltLoadShaderPairWithAttributes("./GLSL/getBackv.c","./GLSL/getBackf.c",2,0,"vVertex",1,"vColor");
	this ->getBackMVP = glGetUniformLocation(this ->getBack,"mvp");//在着色器进行编译和连接之后可以使用glGetUniformLocation函数来寻找统一值：
	//////////////////////////////////////////////
	this ->ultr = gltLoadShaderPairWithAttributes("./GLSL/ultr3Dv.c","./GLSL/ultr3Df.c",2, 0,"vVertex",1,"vColor");//设置着色器
    this ->MVP = glGetUniformLocation(this ->ultr,"mvp");//获取ultr中uniform变量的地址，之后再进行统一值设置
	this ->MV = glGetUniformLocation(this ->ultr,"mv");
	this ->TEX = glGetUniformLocation(this ->ultr,"tex");
	this ->BACKTEX = glGetUniformLocation(this ->ultr,"backtex");
	this ->STEP = glGetUniformLocation(this ->ultr,"step");
	this ->WIDTH = glGetUniformLocation(this ->ultr,"width");
	this ->HEIGHT = glGetUniformLocation(this ->ultr,"height");
	this ->TF = glGetUniformLocation(this ->ultr,"tf");
	this ->Ambient = glGetUniformLocation(this ->ultr,"ambient");
	this ->Diffuse = glGetUniformLocation(this ->ultr,"diffuse");
	this ->Specular = glGetUniformLocation(this ->ultr,"specular");
	this ->BACKCOLOR = glGetUniformLocation(this ->ultr,"backgroundColor");
	this ->LIGHTPOSITIONAFTER = glGetUniformLocation(this ->ultr,"lightPositionAfterVec3");
	this ->EYEPOSITIONAFTER = glGetUniformLocation(this ->ultr,"eyePositionVec3");
	this ->Light = glGetUniformLocation(this ->ultr,"light");
	this ->GradientStep = glGetUniformLocation(this ->ultr,"gradientStep");
	this ->COLORTEX = glGetUniformLocation(this ->ultr,"colortex");
	this ->GILLTEX = glGetUniformLocation(this ->ultr,"gIllTex");

	this ->SSP = glGetUniformLocation(this ->ultr,"surfaceShadingParameter");
	this ->EXPOSURE = glGetUniformLocation(this ->ultr,"exposure");
	this ->GAMMACORRECTION = glGetUniformLocation(this ->ultr,"gammaCorrection");
	this ->KL = glGetUniformLocation(this ->ultr,"KL");
	this ->COLORBOX = glGetUniformLocation(this ->ultr,"colorBox");
	this ->HDRon = glGetUniformLocation(this ->ultr,"hdrOn");
	
	this ->USETFCOLORS = glGetUniformLocation(this ->ultr,"useTFColors");
	this ->RANDOMTEX = glGetUniformLocation(this ->ultr,"random");
	this ->DATATOEXACT = glGetUniformLocation(this ->ultr,"dataToExact");
}
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
//resize opengl window ,every time,upadate(),first call this fun
/*****************************************************************************/
void ultr3D::resizeGL(int width,int height)
{

	if(0 == height)
	{
		height = 1;
	 }
	width = height;
	glViewport(0,0,width,height);

	glBindTexture(GL_TEXTURE_2D,this ->texColorbuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,this->width(),this->height(),0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, this->width(), this->height()); 


}
/**************************************************************************/
//paint the opengl win
/**************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ultr3D::paintGL()
{	
	

	glClearColor(this ->backColor[0],this ->backColor[1],this ->backColor[2],1.0f);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,this ->TexObj);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,this ->texColorbuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D,this ->TFTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,this ->ColorTex);


	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_3D,this ->GIllTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D,this ->RandomTex);

	//set time start for FPS
	glBeginQuery(GL_TIME_ELAPSED,this ->queryFPS);

	//Change CUDA 
	if(this ->LPChange)
	{
		this ->lightAndViewChange();
		this ->LPChange = false;
	}
	if(this ->TFChange)
	{
		this ->tfChange();
		this ->TFChange = false;
	}
	//CUDA CALCULATE THE GLOBAL LIGHT/////////////////////////////

	float* c = (float*)malloc(sizeof(float)*10);
    c[0] = 0;c[1] = 0;c[2] = 0;
	//direct illumination
	if(noLight != this ->light && localLight != this ->light)
	{
        this ->calculateGIVolume(c);
	
	 free(c);
	///////////////////////////////////////////////////////////////
	//update PBO and direction illumination texture
	
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,this ->GIllTexPBO);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_3D,this ->GIllTex);
		glTexImage3D(GL_TEXTURE_3D,0,IllVolDataFormat,this ->IllVolumeWidth,this ->IllVolumeHeight,this ->IllVolumeDepth,0,GL_RGBA,GL_FLOAT,NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
	}

    //render the back of cube
	glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
	glCullFace(GL_FRONT);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glUseProgram(getBack);
	glUniformMatrix4fv(getBackMVP,1,GL_FALSE,mvpPipline.GetModelViewProjectionMatrix());
	cube.Draw();
	glBindFramebuffer(GL_FRAMEBUFFER,0);

    //render
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(ultr);
	glUniformMatrix4fv(MVP,1,GL_FALSE,mvpPipline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(MV,1,GL_FALSE,mvpPipline.GetModelViewMatrix());
	glUniform1i(this ->TEX,0); //一个单独的标量和向量数据类型可以在glUniform函数中使用下面的变量进行设置：
	glUniform1i(this ->BACKTEX,1);
	glUniform1i(this ->WIDTH,this->width());
	glUniform1i(this ->HEIGHT,this->height());
	glUniform1i(this ->STEP,Step);
	glUniform1i(this ->TF,2);
	glUniform3fv(this ->Ambient,1,ambient);
	glUniform3fv(this ->Diffuse,1,diffuse);
	glUniform3fv(this ->Specular,1,specular);
	glUniform3fv(this ->BACKCOLOR,1,this ->backColor);
	glUniform3fv(this ->LIGHTPOSITIONAFTER,1,this ->lightPositionFixedAfterVec3);
	glUniform3fv(this ->EYEPOSITIONAFTER,1,this ->eyePositionAfterVec3);
	glUniform1i(this ->Light,light);
	glUniform1i(this ->GradientStep,gradientStep);
	glUniform1i(this ->COLORTEX,3);
	glUniform1i(this ->GILLTEX,5);
	glUniform1f(this ->SSP,this ->surfaceShadingParameter);
	glUniform1f(this ->EXPOSURE,this ->exposure);
	glUniform1f(this ->GAMMACORRECTION,this ->gammaCorrection);
	glUniform1f(this ->KL,this ->kl);
	glUniform1f(this ->COLORBOX ,this ->colorBox);
	glUniform1i(this ->HDRon,this ->hdrOn);
	glUniform1i(this ->USETFCOLORS,this ->useTFColors);
	glUniform1i(this ->RANDOMTEX,6);
	glUniform1f(this ->DATATOEXACT,this ->dataToExact);
	cube.Draw();
   
	//set the time end of Rendering and calculate the FPS
	glEndQuery(GL_TIME_ELAPSED);
	glGetQueryObjectuiv(this ->queryFPS,GL_QUERY_RESULT,&this ->fps);
	this ->sendFPS(this ->fps);
}


////////////////////////////////////////////////////////////////////////////////	
/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/**************************************************/
//read .raw file
/*************************************************/
//read ultr file
void ultr3D::readUltrRawFile(const char *File )throw(V3dException)
{

	FILE *fp;
	fp = fopen(File, "rb");
	if(NULL == fp)
	{
		qDebug() << "....................................................read ultrfile error";
		return;
	}

	fread(&this ->TexW,sizeof(int),1,fp);
	fread(&this ->TexH,sizeof(int),1,fp);
	fread(&this ->TexD,sizeof(int),1,fp);
	//for most ultrdata,data is 8 bit
	this ->dataType = unsignedChar;
    qDebug()<<"w:"<<TexW<<"h:"<<TexH<<"d:"<<TexD;
	this ->dataSize = this ->TexW * this ->TexH * this ->TexD;
	this ->dataSizeOfByte = this ->dataSize * this ->dataType;
	data = (GLubyte*)malloc(this ->dataSizeOfByte);
	fread(data,this ->dataSizeOfByte,1,fp);
	if(0 == this ->dataSize)
		throw dataFormException();
	fclose(fp);
	fp = NULL;
}


//read CT file
void  ultr3D::readCTRawFile(const char *File,int width,int height,int depth)throw(V3dException)
{
	FILE *fp;
	fp = fopen(File, "rb");
	if(NULL == fp)
	{
		qDebug() << "....................................................read CTfile error";
		return;
	}
	
	this ->TexW = width;
	this ->TexH = height;
	this ->TexD = depth;
    qDebug()<<"w:"<<TexW<<"h:"<<TexH<<"d:"<<TexD;
	this ->dataSize = this ->TexW * this ->TexH * this ->TexD;
    this ->dataSizeOfByte = this ->dataSize * this ->dataType;
	
	data = (GLubyte*)malloc(this ->dataSizeOfByte);
    fread(data,this ->dataSizeOfByte,1,fp);
	if(0 == this ->dataSize)
		throw dataFormException();
	fclose(fp);
	fp = NULL;
}


/************************************************/
//load3DTexture
/************************************************/
bool ultr3D::load3DTex()
{

	
	glGenTextures(1,&TexObj);
	glBindTexture(GL_TEXTURE_3D,TexObj);
	
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D,0,GL_ALPHA,this ->TexW,this ->TexH,this ->TexD,0,GL_ALPHA,dataFormat,data);

	return true;
}


/*************************************************/
//mouseEvent , keyEvent and closeEvent
/*************************************************/
void ultr3D::mousePressEvent(QMouseEvent *e)
{
	mouseOldX = e->x();
	mouseOldY = e->y();
	if(e ->button() == Qt::LeftButton)
	{
		this ->leftMouse = true;
		this ->rightMouse = false;
	}else if(e ->button() == Qt::RightButton)
	{
		this ->leftMouse = false;
		this ->rightMouse = true;
	}
	
}
void ultr3D::closeEvent(QCloseEvent *event)
{
	this ->hide();
}
void ultr3D::mouseMoveEvent(QMouseEvent *e)
{
	
	int deltax =  mouseOldX - e->x();
	int deltay =  mouseOldY - e->y();
	if(true == this ->leftMouse)
	{
		//	qDebug() <<deltax<<".........."<<deltay;

		float yangle = 11*deltax/(this -> width());
		float xangle = 11*deltay/(this -> height());

		this ->rotateAngle(xangle,-1.0f,0.0f,0.0f);
		this ->rotateAngle(yangle,0.0f,-1.0f,0.0f);
		
		//float xangle = - 11 * deltax / (this->width()) ;
		//float yangle = -11 * deltay / (this->height()) ;
		//this->rotateAngle(yangle, 1.0f, 0.0f, 0.0f);
		//this->rotateAngle(xangle, 0.0f, 1.0f, 0.0f);
		//this->rotateAngle(0.0f, 0.0f, 0.0f, 1.0f);

		this ->isLPChange(true);
		emit rotate(xangle,yangle,0.0);
		this->update();
	}else if(true == this ->rightMouse)
	{
		if(deltay > 0)
		{
			this ->zoom(1.01f);
		}else
		{
			this ->zoom(1.0f/1.01f);
		}
		this ->isLPChange(true);
		this ->update();
	}

}
/*
void ultr3D::keyPressEvent(QKeyEvent *e)
{
	switch(e ->key())
	{
	
	case Qt::Key_Q:
		
		break;
	case Qt::Key_W:
		
		break;
	}
}
*/
/**************************************************/
//make a cube
/**************************************************/
void ultr3D::makeCube()
{	
	cube.Begin(GL_QUADS,24,0);
//	    cube.Color4f(0.0f,1.0f,0.0f,1.0f);
//	    cube.Vertex3f(-1.0f,1.0f,1.0f);		
//        cube.Color4f(0.0f,0.0f,0.0f,1.0f);
//		cube.Vertex3f(-1.0f,-1.0f,1.0f);
//        cube.Color4f(1.0f,0.0f,0.0f,1.0f);
//		cube.Vertex3f(1.0f,-1.0f,1.0f);	
//        cube.Color4f(1.0f,1.0f,0.0f,1.0f);
//        cube.Vertex3f(1.0f,1.0f,1.0f);
///////////////////////////////////////////////////////
//        cube.Color4f(1.0f,1.0f,0.0f,1.0f);
//		cube.Vertex3f(1.0f,1.0f,1.0f);	
//        cube.Color4f(1.0f,0.0f,0.0f,1.0f);
//		cube.Vertex3f(1.0f,-1.0f,1.0f);
//        cube.Color4f(1.0f,0.0f,1.0f,1.0f);
//		cube.Vertex3f(1.0f,-1.0f,-1.0f);
//        cube.Color4f(1.0f,1.0f,1.0f,1.0f);
//		cube.Vertex3f(1.0f,1.0f,-1.0f);		
///////////////////////////////////////////////////////		
//		cube.Color4f(1.0f,1.0f,1.0f,1.0f);
//		cube.Vertex3f(1.0f,1.0f,-1.0f);		
//        cube.Color4f(1.0f,0.0f,1.0f,1.0f);
//		cube.Vertex3f(1.0f,-1.0f,-1.0f);	
//        cube.Color4f(0.0f,0.0f,1.0f,1.0f);
//		cube.Vertex3f(-1.0f,-1.0f,-1.0f);		
//        cube.Color4f(0.0f,1.0f,1.0f,1.0f);
//		cube.Vertex3f(-1.0f,1.0f,-1.0f);		
//////////////////////////////////////////////////////
//        cube.Color4f(0.0f,1.0f,1.0f,1.0f);
//		cube.Vertex3f(-1.0f,1.0f,-1.0f);	
//        cube.Color4f(0.0f,0.0f,1.0f,1.0f);
//		cube.Vertex3f(-1.0f,-1.0f,-1.0f);
//        cube.Color4f(0.0f,0.0f,0.0f,1.0f);
//		cube.Vertex3f(-1.0f,-1.0f,1.0f);
//        cube.Color4f(0.0f,1.0f,0.0f,1.0f);
//		cube.Vertex3f(-1.0f,1.0f,1.0f);
///////////////////////////////////////////////////////
//        cube.Color4f(0.0f,1.0f,1.0f,1.0f);
//		cube.Vertex3f(-1.0f,1.0f,-1.0f);
//        cube.Color4f(0.0f,1.0f,0.0f,1.0f);
//		cube.Vertex3f(-1.0f,1.0f,1.0f);
//        cube.Color4f(1.0f,1.0f,0.0f,1.0f);
//		cube.Vertex3f(1.0f,1.0f,1.0f);
//        cube.Color4f(1.0f,1.0f,1.0f,1.0f);
//		cube.Vertex3f(1.0f,1.0f,-1.0f);	
///////////////////////////////////////////////////////
//        cube.Color4f(0.0f,0.0f,0.0f,1.0f);
//		cube.Vertex3f(-1.0f,-1.0f,1.0f);
//        cube.Color4f(0.0f,0.0f,1.0f,1.0f);
//		cube.Vertex3f(-1.0f,-1.0f,-1.0f);
//        cube.Color4f(1.0f,0.0f,1.0f,1.0f);
//		cube.Vertex3f(1.0f,-1.0f,-1.0f);
//        cube.Color4f(1.0f,0.0f,0.0f,1.0f);
//		cube.Vertex3f(1.0f,-1.0f,1.0f);	
////////////////////////////////////////////////////
	cube.Color4f(0.0f, 1.0f, 0.0f, 1.0f);
	cube.Vertex3f(-CubeW, CubeH, CubeD);
	cube.Color4f(0.0f, 0.0f, 0.0f, 1.0f);
	cube.Vertex3f(-CubeW, -CubeH, CubeD);
	cube.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
	cube.Vertex3f(CubeW, -CubeH, CubeD);
	cube.Color4f(1.0f, 1.0f, 0.0f, 1.0f);
	cube.Vertex3f(CubeW, CubeH, CubeD);
	/////////////////////////////////////////////////////
	cube.Color4f(1.0f, 1.0f, 0.0f, 1.0f);
	cube.Vertex3f(CubeW, CubeH, CubeD);
	cube.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
	cube.Vertex3f(CubeW, -CubeH, CubeD);
	cube.Color4f(1.0f, 0.0f, 1.0f, 1.0f);
	cube.Vertex3f(CubeW, -CubeH, -CubeD);
	cube.Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	cube.Vertex3f(CubeW, CubeH, -CubeD);
	/////////////////////////////////////////////////////		
	cube.Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	cube.Vertex3f(CubeW, CubeH, -CubeD);
	cube.Color4f(1.0f, 0.0f, 1.0f, 1.0f);
	cube.Vertex3f(CubeW, -CubeH, -CubeD);
	cube.Color4f(0.0f, 0.0f, 1.0f, 1.0f);
	cube.Vertex3f(-CubeW, -CubeH, -CubeD);
	cube.Color4f(0.0f, 1.0f, 1.0f, 1.0f);
	cube.Vertex3f(-CubeW, CubeH, -CubeD);
	////////////////////////////////////////////////////
	cube.Color4f(0.0f, 1.0f, 1.0f, 1.0f);
	cube.Vertex3f(-CubeW, CubeH, -CubeD);
	cube.Color4f(0.0f, 0.0f, 1.0f, 1.0f);
	cube.Vertex3f(-CubeW, -CubeH, -CubeD);
	cube.Color4f(0.0f, 0.0f, 0.0f, 1.0f);
	cube.Vertex3f(-CubeW, -CubeH, CubeD);
	cube.Color4f(0.0f, 1.0f, 0.0f, 1.0f);
	cube.Vertex3f(-CubeW, CubeH, CubeD);
	/////////////////////////////////////////////////////
	cube.Color4f(0.0f, 1.0f, 1.0f, 1.0f);
	cube.Vertex3f(-CubeW, CubeH, -CubeD);
	cube.Color4f(0.0f, 1.0f, 0.0f, 1.0f);
	cube.Vertex3f(-CubeW, CubeH, CubeD);
	cube.Color4f(1.0f, 1.0f, 0.0f, 1.0f);
	cube.Vertex3f(CubeW, CubeH, CubeD);
	cube.Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	cube.Vertex3f(CubeW, CubeH, -CubeD);
	/////////////////////////////////////////////////////
	cube.Color4f(0.0f, 0.0f, 0.0f, 1.0f);
	cube.Vertex3f(-CubeW, -CubeH, CubeD);
	cube.Color4f(0.0f, 0.0f, 1.0f, 1.0f);
	cube.Vertex3f(-CubeW, -CubeH, -CubeD);
	cube.Color4f(1.0f, 0.0f, 1.0f, 1.0f);
	cube.Vertex3f(CubeW, -CubeH, -CubeD);
	cube.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
	cube.Vertex3f(CubeW, -CubeH, CubeD);
	////////////////////////////////////////////////////
	cube.End();
}
/************************************************************/
//get graycount in the texture
/***********************************************************/
void ultr3D::getGrayCount()
{
	//QVector initialize to 0
	unsigned int countnum = 1;
	countnum = 1 << (this ->dataType * 8);
	unsigned int countnumbyte = countnum * sizeof(unsigned int);
    this ->grayCount = (unsigned int*)malloc(countnumbyte);
	memset(this ->grayCount, 0, countnumbyte);
	this ->mostHighX = countnum - 1;
	this ->mostHigh = 0;
	if(unsignedChar == this ->dataType)
	{
		for(int i = 0;i < this ->dataSize;i++)
			grayCount[((GLubyte*)data)[i]]++;
	}
	if(unsignedShort == this ->dataType)
	{
		int index = 0;
		for(int i = 0;i < this ->dataSize;i++)
		{
			index = ((GLushort*)data)[i];
			//qDebug() <<"    "<<((GLushort*)data)[i];
	
			(this ->grayCount)[index] = (this ->grayCount)[index] + 1;
		}
	}
	if(unsignedInt == this ->dataType)
	{
		for(int i = 0;i < this ->dataSize;i++)
			grayCount[((GLuint*)data)[i]]++;
	}
	for(int i = countnum - 1;i > 0;i--)
	{ 
		
		if((this ->grayCount)[i] > 3 && (this ->grayCount)[i - 1] > 3)
			break;

		this ->mostHighX --;
	//	qDebug() << (this ->grayCount)[i];
		
	}
	for(int i = 0;i < this ->mostHighX;i++)
	{
		
		if((this  ->grayCount)[i] > this ->mostHigh)
		{
			this ->mostHigh = (this  ->grayCount)[i];
		}
	}
	this ->dataToExact = (countnum - 1) * 1.0/ (this ->mostHighX );
	//qDebug() << this ->mostHighX;
}
/***********************************************/
//load the texture of scattering
/***********************************************/
//void ultr3D::loadScatteringTex()
//{
//	GLubyte scatteringData[256];
//	for(int i = 0;i < 256;i++)
//	{		
//		scatteringData[i] = 0;
//	}
//	this ->scatteringArray = InitialScatteringData(scatteringData);
//
//	
//}
/************************************************/
//CUDA SCATTERING COEFF CHANGE
/************************************************/
void ultr3D::cudaScatterChange()
{
	/*GLubyte scatteringData[256];
	int k = 255 * this ->scatteringMost;
	
	for(int i = k;i < 256;i++)
	{
		scatteringData[i] = 255;
	}
	for(int i = 0;i < k;i++)
	{		
		scatteringData[i] = 0 ;
	}
	checkCudaErrors(cudaMemcpyToArray(this ->scatteringArray,0,0,scatteringData,256,cudaMemcpyHostToDevice));*/


}
/**************************************************/
//load transform function texture
/**************************************************/
bool ultr3D::loadTFTex()
{

    if( 0 == this ->TFTex)
    {
		glGenTextures(1,&this ->TFTex);
		glBindTexture(GL_TEXTURE_1D,this ->TFTex);

		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_UNSIGNED_BYTE,this ->tfData);
		
	}else
	{
		glBindTexture(GL_TEXTURE_1D,this ->TFTex);
		glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_UNSIGNED_BYTE,this ->tfData);

		this ->TFChange = true;

	}
	return true;
}


/**************************************************/
//load color map
/**************************************************/
bool ultr3D::loadColorTex()
{

	QImage color("color.png");
	if(color.isNull())
	{
		qDebug() <<"that have a error in load color texture";
		return false;

	}
	int height = color.height();
	int width = color.width();
	
	//colorData = (GLubyte*)malloc(height * width * 4);
	colorData = (GLubyte*)color.bits();

	if(0 == this ->ColorTex)
	{
		glGenTextures(1,&this ->ColorTex);
		glBindTexture(GL_TEXTURE_2D,this ->ColorTex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,colorData);

	}else
	{
		glBindTexture(GL_TEXTURE_2D,this ->ColorTex);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,colorData);
	}

	return true;
}
/**************************************************/
//load random texture
/**************************************************/
bool ultr3D::loadRandomTex()
{
	FILE* fp;
	fp = fopen("random.tex","rb");
	if(NULL == fp)
	{
        qDebug() << "have a bug on load random texture";
        return 0;
     }
	this ->random = (GLubyte*)malloc(256 * 256); 
	fread(this ->random,256*256,1,fp);
    fclose(fp);

	glGenTextures(1,&this ->RandomTex);
	glBindTexture(GL_TEXTURE_2D,this ->RandomTex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA,256,256,0,GL_ALPHA,GL_UNSIGNED_BYTE,this ->random);

	return true;
}

/**************************************************/
//Initialize PBO use cuda to produce the direct illumination and indirect illumination
/**************************************************/
void ultr3D::InitGetGlobalIllum()throw(V3dException)
{
	//global illumination PBO;///////////////////////////////////////////////
	glGenBuffers(1,&this ->GIllTexPBO);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,this ->GIllTexPBO);
	glBufferData(GL_PIXEL_PACK_BUFFER,this ->IllVolumeSizeOfByte,NULL,GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

	if(glGetError() != GL_NO_ERROR)
	    throw(MemoryException());

	//global illumination Texture
	glGenTextures(1,&this ->GIllTex);
	glBindTexture(GL_TEXTURE_3D,this ->GIllTex);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D,0,IllVolDataFormat,this ->IllVolumeWidth,this ->IllVolumeHeight,this ->IllVolumeDepth,0,GL_RGBA,GL_FLOAT,NULL);

	glBindTexture(GL_TEXTURE_3D,0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);

	if(glGetError() != GL_NO_ERROR)
	    throw(MemoryException());
	//////////////////////////////////////////////////////////////////////////
	//INITIAL THE MEMORY OF CUDA//////////////////////////////////////////////
	//data tex
	this ->cuArray = InitialCudaData(this ->data,this ->TexW,this ->TexH,this ->TexD,this ->dataType,this ->dataToExact);
	//tf tex
    GLubyte* _tfdata = (GLubyte*)malloc(256);
	for(int i = 0;i < 256;i++)
	{
		
		_tfdata[i] = this ->tfData[i*4+3];
	}
	this ->tfArray = InitialCudaTranData(_tfdata);
	free(_tfdata);
	_tfdata = NULL;

	setSizeParameters(this ->IllVolumeWidth,this ->IllVolumeHeight,this ->IllVolumeDepth);
	setDataSizeParameters(this ->TexW,this ->TexH,this ->TexD);
	/////////////////////////////////////////////////////////////////////////
	//initialize the position of light and object and ao paratemeter
	this ->lightAndViewChange();

	//used for the light surface
	int most;
	if(this ->IllVolumeWidth >= this ->IllVolumeHeight && this ->IllVolumeWidth >= this ->IllVolumeDepth)
		most = this ->IllVolumeWidth;
	else if(this ->IllVolumeHeight >= this ->IllVolumeWidth && this ->IllVolumeHeight >= this ->IllVolumeDepth)
		most = this ->IllVolumeHeight;
	else
		most = this ->IllVolumeDepth;
	initialLightSurface( most,most,SURFACE_BYTES * 8,this ->surfaceArray,this ->surfaceArray2);
	(this ->surfaceSize)[0] = most;
	(this ->surfaceSize)[1] = most;


}

/**************************************************/
//CUDA LIGHTPOSITION CHANGE
/**************************************************/
void ultr3D::lightAndViewChange()
{
	M3DMatrix44f result,modelviewStackInvert;
	//store the final light position
	M3DVector3f temp;
	M3DVector4f _temp;

	//m3dInvertMatrix44(modelviewStackInvert, this->mvpPipline.GetModelViewProjectionMatrix());
	
	m3dInvertMatrix44(modelviewStackInvert,this ->modelviewStack.GetMatrix());
	m3dTransformVector4(_temp,this ->lightPositionVec4,modelviewStackInvert);
	temp[0] = _temp[0];
	temp[1] = _temp[1];
	temp[2] = - _temp[2];

	m3dLoadVector3(this ->lightPositionAfterVec3,temp[0],temp[1],temp[2]);
	setLightPositionAfter(temp);

	//phong light position fixed
	m3dTransformVector4(_temp,this ->lightPositionFixedVec4,modelviewStackInvert);
	temp[0] = _temp[0];
	temp[1] = _temp[1];
	temp[2] = - _temp[2];

	m3dLoadVector3(this ->lightPositionFixedAfterVec3,temp[0],temp[1],temp[2]);

	//eyePosition change
	m3dTransformVector4(_temp,this ->eyePositionVec4,modelviewStackInvert);
	temp[0] = _temp[0];
	temp[1] = _temp[1];
	temp[2] = - _temp[2];
	m3dLoadVector3(this ->eyePositionAfterVec3,temp[0],temp[1],temp[2]);
	
}
/************************************************/
//CUDA TRANSFORM FUCNTION CHANGE
/************************************************/
void ultr3D::tfChange()
{
	GLubyte* tfdata = (GLubyte*)malloc(256);
	for(int i = 0;i < 256;i++)
	{
		tfdata[i] = this ->tfData[i*4+3];
	}
    this ->updateCudaTranData(tfdata);
	free(tfdata);


}
/********************************************************************************/
//CUDA FUNCTION
/********************************************************************************/
/***********************************************/
//update the texture data
/***********************************************/
void ultr3D::updateCudaTranData(void* tfdata,int offset ,int length)
{
	checkCudaErrors(cudaMemcpyToArray(this ->tfArray,offset,0,tfdata,length,cudaMemcpyHostToDevice));
}
/***********************************************/
//cuda run the direction illumination
/***********************************************/
void ultr3D::calculateGIVolume(float*c)
{
	////////////////test////////////
	float *dev_c = 0;
	checkCudaErrors(cudaMalloc((void**)&dev_c,10 * sizeof(float)));
	///////////////////////////////////
	
    checkCudaErrors(cudaSetDevice(0));
	dim3 block(this ->IllVolumeWidth);
	dim3 grid(this ->IllVolumeHeight,this ->IllVolumeDepth);

	cudaGraphicsGLRegisterBuffer(&resource,this ->GIllTexPBO,cudaGraphicsMapFlagsNone);
	float* pbo = NULL;
	cudaGraphicsMapResources(1,&resource,NULL);
	cudaGraphicsResourceGetMappedPointer((void**)&pbo, &(this->IllVolumeSizeOfByte), resource);

	render_Kernel_GI(grid,block,pbo,this ->lightPositionAfterVec3,this ->surfaceSize,dev_c);
    ////////////////test////////////
	checkCudaErrors(cudaMemcpy(c,dev_c,10* sizeof(float),cudaMemcpyDeviceToHost));
	///////////////////////////////////
    cudaGraphicsUnmapResources(1,&resource,NULL);
    cudaGraphicsUnregisterResource(resource);
   ////////////////test////////////
	cudaFree(dev_c);
   ///////////////////////////////////

}
/***********************************************/
//save the rendering Image
/***********************************************/
void ultr3D::saveImage()
{
	//set the name of file
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("hh_mm_ss");
	QString tempfile = this ->fileName;
	tempfile.replace(".raw",current_date);
	QString str = tempfile + ".png";
	std::string s = str.toUtf8().constData();//.toStdString();
	int PicWidth = this->width() / 4;
	PicWidth = PicWidth * 4;
	int PicHeight = this->height() / 4;
	PicHeight = PicHeight * 4;
	//read image
	//unsigned char* pixels = (unsigned char*)malloc(3 * this ->width() * this ->height());
	unsigned char* pixels = (unsigned char*)malloc(3 * PicWidth * PicHeight);
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	//glReadPixels(0,0,this ->width(),this ->height(),GL_RGB,GL_UNSIGNED_BYTE,pixels);
	glReadPixels(0, 0, PicWidth, PicHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	//turn over the image to get right image
	
	/*for(int x = 0;x < this ->width();x++)
	{
		for(int y = 0;y < (this ->height() - 1) / 2;y++)
		{
			unsigned char temp = pixels[(x + y * this ->height()) * 3 + 0];
			pixels[(x + y * this ->height()) * 3 + 0] = pixels[(x + (this ->height() - 1 - y) * this ->height()) * 3 + 0];
			pixels[(x + (this ->height() - 1 - y) * this ->height()) * 3 + 0] = temp;

			temp = pixels[(x + y * this ->height()) * 3 + 1];
			pixels[(x + y * this ->height()) * 3 + 1] = pixels[(x + (this ->height() - 1 - y) * this ->height()) * 3 + 1];
			pixels[(x + (this ->height() - 1 - y) * this ->height()) * 3 + 1] = temp;

			temp = pixels[(x + y * this ->height()) * 3 + 2];
			pixels[(x + y * this ->height()) * 3 + 2] = pixels[(x + (this ->height() - 1 - y) * this ->height()) * 3 + 2];
			pixels[(x + (this ->height() - 1 - y) * this ->height()) * 3 + 2] = temp;

		
		}
	}*/
	for (int x = 0; x <PicWidth; x++)
	{
		for (int y = 0; y <PicHeight/2; y++)
		{
			unsigned char temp = pixels[(x + y * PicWidth) * 3 + 0];
			pixels[(x + y * PicWidth) * 3 + 0] = pixels[(x + (PicHeight - 1 - y) * PicWidth) * 3 + 0];
			pixels[(x + (PicHeight - 1 - y) * PicWidth) * 3 + 0] = temp;
		    temp = pixels[(x + y * PicWidth) * 3 + 1];
			pixels[(x + y * PicWidth) * 3 + 1] = pixels[(x + (PicHeight - 1 - y) * PicWidth) * 3 + 1];
			pixels[(x + (PicHeight - 1 - y) * PicWidth) * 3 + 1] = temp;
			temp = pixels[(x + y * PicWidth) * 3 + 2];
			pixels[(x + y * PicWidth) * 3 + 2] = pixels[(x + (PicHeight - 1 - y) * PicWidth) * 3 + 2];
			pixels[(x + (PicHeight - 1 - y) * PicWidth) * 3 + 2] = temp;


		}
	}
    //save as png
   // QPixmap pixmap = QPixmap::fromImage(QImage(pixels,this ->width(),this ->height(),QImage::Format_RGB888));
	QPixmap pixmap = QPixmap::fromImage(QImage(pixels, PicWidth, PicHeight, QImage::Format_RGB888));
	QFile file(str);
	file.open(QIODevice::WriteOnly);
	pixmap.save(&file,"PNG");

    //////////////
   free(pixels);
   QMessageBox::information(NULL, "Save as", str, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}
void ultr3D::rotateAngle(const float& angle, const float& x, const float& y, const float& z)
{

	//this->modelviewStack.Rotate(angle , x, y, z);
	this->modelviewStack.RotateToWorld(angle, x, y, z);

	
}
void ultr3D::autoRotate()
{
	this ->rotateAngle(1.0,0.0,1.0,0.0);
	this->isLPChange(true);
	//emit rotate(1.0, 0.0, 1.0);
	this ->update();
	//emit rotate(0.0,0.0,1.0);
}