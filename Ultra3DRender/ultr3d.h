#pragma once
#ifndef ULTR3D_H
#define ULTR3D_H
//opengl lib

//#pragma comment(lib,"glew32.lib")
//#pragma comment(lib,"glew32s.lib")
//#pragma comment(lib,"gltools.lib")
//#pragma comment(lib,"freeglut.lib")
//#pragma comment(lib,"glut.lib")
//#pragma comment(lib,"glut32.lib")
//#pragma comment(lib,"ws2_32.lib")
//Qt lib

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <GLTools.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <GLFrame.h>
#include <stdio.h>
#include <QtOpenGL/QGLWidget>
#include <QtWidgets/QOpenglWidget>
#include <QtGui/qopenglext.h>
#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>
#include <QtCore/QVector>
#include <QtCore/QTime>
#include <QtGui/QImage>
#include <QtGui/QRgb>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtCore/QFile>
//#include <QtGui>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include "v3dException.h"
#include <QtCore/QThread>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
//CUDA utilities
#include <helper_cuda.h>


//PFNGLBUFFERDATAARBPROC    glBufferData = NULL;
enum LightStyle 
{
	noLight = 0, 
	localLight, 
	directLight, 
	indrectLight, 
	globalLight, 
	directPhongIllum, 
	globalPhongIllum, 
	indirectPhongIllum 
};
//dataType = 1 for unsinged char 2 for unsigned short
enum DataType
{
	unsignedChar = 1,
	unsignedShort, 
	unsignedInt = 4
};

class ultr3D : public /*QOpenGLWidget*/QGLWidget
{
	Q_OBJECT

public:
	ultr3D(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	ultr3D(QString fileName, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	ultr3D(QString fileName,int width,int height,int depth, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	 ~ultr3D();


    GLBatch cube;
	GLMatrixStack modelviewStack;
	GLFrustum projection;
	GLMatrixStack projectionStack;
	GLGeometryTransform mvpPipline;
	GLFrame mymodelview;


	GLuint  ultr;
	//getBack Shader /////////////////////////
	GLuint getBack;
	GLuint framebuffer;
	GLuint texColorbuffer;
	GLuint rbo;
	GLint getBackMVP;
  
	GLint MVP;
	GLint MV;
	GLint TEX;
	GLint BACKTEX;
	GLint STEP;
	GLint WIDTH;
	GLint HEIGHT;

	
	int Step;
	int gradientStep;
	GLint GradientStep;
	
	float rotXOld;
	float rotYOld;

	float CubeW, CubeH, CubeD;

	//the quantity of every gray level in the texture
	unsigned int* grayCount;
	unsigned int mostHighX;
	unsigned int mostHigh;
	//count the grayCount
	void getGrayCount();

	float dataToExact;
	GLint DATATOEXACT;
	
    //the h,w,d of volume tex
	GLint TexH;
	GLint TexW;
	GLint TexD;

	// phong - light model
	M3DVector3f ambient;
	M3DVector3f diffuse;
	M3DVector3f specular;
	M3DVector4f lightPositionVec4;
	M3DVector3f lightPositionAfterVec3;
	M3DVector4f lightPositionFixedVec4;
    M3DVector4f lightPositionFixedAfterVec3;
	M3DVector3f backColor;
	void setBackColor(const float& r,const float& g,const float& b)
	{
		m3dLoadVector3(this ->backColor,r,g,b);
	}

	GLint Ambient;
	GLint Diffuse;
	GLint Specular;
	GLint LIGHTPOSITIONAFTER;
	GLint BACKCOLOR;

	GLint EYEPOSITIONAFTER;
	
	M3DVector4f eyePositionVec4;
	M3DVector3f eyePositionAfterVec3;
	//turn on the light
	GLint Light;
	int light;
	//void setLight(int)
	//light transform matrix
	M3DMatrix44f lightModelViewMatrix;
	//sacling matrix
	GLMatrixStack sacling;
	

	//surface shading parameter
	GLint SSP;
	float surfaceShadingParameter;
	void setSurfaceShadingParameter(const float& _s)
	{   this ->surfaceShadingParameter = _s; }
	

	QString fileName;


public:

	//read raw data
	void readRawFile(const char *File);
	void readUltrRawFile(const char *File)throw(V3dException);
    void readCTRawFile(const char *File,int width,int height,int depth)throw(V3dException);
	
	//set sample step
	void setSampleStep(int _Step){Step = _Step;}
	//set gradient step
	void setGradientStep(int _gradientStep){gradientStep = _gradientStep;}

	//rotate
	void rotateAngle(const float& angle,const float& x,const float& y,const float& z);

	
	//transform function texture;
	GLubyte* tfData;
	GLubyte* getTFData() const
	{
		return this ->tfData;
	}
	bool loadTFTex();
	//color mapping texture;
	GLubyte* colorData;
	bool loadColorTex();
	GLuint ColorTex;
	GLint COLORTEX;

	//rand texture
	GLubyte* random;
	bool loadRandomTex();
	GLuint RandomTex;
	GLint RANDOMTEX;

	int AngleNum = 0;
	bool TFChange;
	bool LPChange;
	void isLPChange(const bool& _LPChange)
	{
		this ->LPChange = _LPChange;
	}
	//useTF colors 
	GLint USETFCOLORS;
	bool useTFColors;
	void isUseTFColors(const bool& _useTFColors)
	{
		this ->useTFColors = _useTFColors;	
	}

    //HDR is on/off
    GLint HDRon;
	bool hdrOn;
	void isHdrOn(const bool& _hdrOn){this ->hdrOn = _hdrOn;}
	
	//HDR exposure parameter
	GLint EXPOSURE;
	float exposure;
	void setExposure(const float& _exposure){this ->exposure = _exposure;}
	//Gamma correction
	GLint GAMMACORRECTION;
	float gammaCorrection;
	void setGammaCorrection(const float& _gamma){this ->gammaCorrection = _gamma;}

	//light attenuation
	GLint KL;
	float kl;
	void setKl(const float& _kl){this ->kl = _kl;}
	//color Box
	GLint COLORBOX;
	float colorBox;
	void setColorBox(const float& _colorBox){this ->colorBox = _colorBox;}

	 //Volume Texture
	void* data;
	unsigned int dataSize;
	unsigned int dataSizeOfByte;
	
	DataType dataType;

	//scattering coeff
	float scatteringMost;
	//load the scale of scattering
	void loadScatteringTex();
	void cudaScatterChange();


	//save Image
	void saveImage();
	//zoom in and zoom out
	void zoom(float value = 1.0f)
	{
		//////////////////////////
		//modelviewStack.ScaleToWorld(value,value,value);
		//sacling.ScaleToWorld(value,value,value);
		modelviewStack.Scale(value, value, value);
		sacling.Scale(value,value,value);
	}
	//save and load modelviewStack
	
public slots:
	void autoRotate();
private:
	
	//tf Texture
	GLuint TFTex;
	GLint TF;

	//	GLint volume Tex;
	GLuint TexObj;

	//mouse event
	int mouseOldX;
	int mouseOldY;
	
	//query FPS
	GLuint queryFPS;
	unsigned int fps;

	//for scale the image
	bool leftMouse;
	bool rightMouse;

	//use to produce the global texture
	GLuint GIllTexPBO;
	GLuint GIllTex;
	GLint GILLTEX;

	//Illumination volume size
	int IllVolumeWidth;
	int IllVolumeHeight;
	int IllVolumeDepth;
	int IllVolumeType;
	unsigned int IllVolumeSize;
	size_t IllVolumeSizeOfByte;
    

	//CUDA VOlUME DATA
	cudaArray *cuArray;
	cudaArray *tfArray;
	cudaArray *scatteringArray;
	cudaArray *surfaceArray;
	cudaArray *surfaceArray2;
	int surfaceSize[2];

	cudaGraphicsResource *resource;




protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width,int height);

	//void paintEvent(QPaintEvent * event);
   
	void makeCube();

	bool load3DTex();

	
	//initialize data
	void initializeData();
	//delete ultr3D
	void deleteData();


	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void closeEvent(QCloseEvent *event);
	//void keyPressEvent(QKeyEvent *e);

	//produce global illuminaton
	void InitGetGlobalIllum()throw(V3dException);

	//LIGHT CHANGE
	void lightAndViewChange();
	//CUDA TF CHANGE
	void tfChange();

signals:
	void rotate(float,float,float);
	void sendFPS(unsigned int);
	
//CUDA FUNCTION
private:
   //update the texture data
   void updateCudaTranData(void* tfdata,int offset = 0,int length = 256);

   //calculate the light volume
   void calculateGIVolume(float*c);
  
	
};

#endif 
