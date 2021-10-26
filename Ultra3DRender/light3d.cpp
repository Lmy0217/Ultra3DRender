#include "light3d.h"



using namespace std;
light3D::light3D(QWidget *parent , Qt::WindowFlags flags ):QGLWidget(parent)
{

	//setGeometry(QRect(550, 100, 50, 50));
	setFocusPolicy(Qt::StrongFocus);

	m3dLoadVector3(ambient,1.0,1.0,1.0);
	m3dLoadVector3(diffuse,1.0,1.0,1.0);
	m3dLoadVector3(specular,1.0,1.0,1.0);

	m3dLoadVector3(position,0.0,0.0,1.0);
	this ->ambientIn = 0.0;
	this ->diffuseIn = 1.0;
	this ->specularIn = 0.0;
	this ->distance = 2.5;
	this ->notifChange = true;
}

light3D::~light3D()
{
}


void light3D::initializeGL()
{
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		qDebug("glewInit error: %s\nglew version: %s", glewGetErrorString(err), glewGetString(GLEW_VERSION));
		return;
	}

	gltMakeCube(cubeBatch,1.0f);
	gltMakeSphere(sphereBatch,1.74f,56,56);
	

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	

	this ->projection.SetOrthographic(-2.0f,2.0f,-2.0f,2.0f,-2.0f,2.0f);
	this ->projectionStack.LoadMatrix(projection.GetProjectionMatrix());

	this ->spherePipline.SetMatrixStacks(sphereModelViewStack,projectionStack);
	this ->cubePipline.SetMatrixStacks(modelViewStack,projectionStack);
	//cubeModelViewStack.RotateToWorld(90.0f,1.0f,1.0f,1.0f)

	this ->sphereShader = gltLoadShaderPairWithAttributes("./GLSL/spherevp.c","./GLSL/spherefp.c",2,0,"vVertex",2,"vNormal");
	this ->cubeShader = gltLoadShaderPairWithAttributes("./GLSL/cubevp.c","./GLSL/cubefp.c",1.0,"vVertex");


    this ->cubeMVP = glGetUniformLocation(cubeShader,"mvp");
	this ->sphereMVP = glGetUniformLocation(sphereShader,"mvp");
	this ->sphereMV = glGetUniformLocation(sphereShader,"mv"); 
	this ->Ambient = glGetUniformLocation(sphereShader,"ambient");
	this ->Diffuse = glGetUniformLocation(sphereShader,"diffuse");
	this ->Specular = glGetUniformLocation(sphereShader,"specular");
	this ->AmbientI = glGetUniformLocation(sphereShader,"ambienti");
	this ->DiffuseI = glGetUniformLocation(sphereShader,"diffusei");
	this ->SpecularI = glGetUniformLocation(sphereShader,"speculari");
	this ->Distance = glGetUniformLocation(sphereShader,"distance");

}

void light3D::resizeGL(int width,int height)
{
	if( 0 == height)
	{

		height = 1;
	}
	glViewport(0,0,width,height);
}

void light3D::paintGL()
{


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(cubeShader);
	glUniformMatrix4fv(cubeMVP,1,GL_FALSE,cubePipline.GetModelViewProjectionMatrix());

	this ->cubeBatch.Draw();

	glUseProgram(sphereShader);
	glUniformMatrix4fv(sphereMVP,1,GL_FALSE,spherePipline.GetProjectionMatrix());
	glUniformMatrix4fv(sphereMV,1,GL_FALSE,spherePipline.GetModelViewMatrix());
	glUniform3fv(Ambient,1,ambient);
	glUniform3fv(Diffuse,1,diffuse);
	glUniform3fv(Specular,1,specular);
	glUniform1f(AmbientI,ambientIn);
	glUniform1f(DiffuseI,diffuseIn);
	glUniform1f(SpecularI,specularIn);
	glUniform1f(Distance,distance);

	this ->sphereBatch.Draw();
	if(this ->notifChange)
	{
	   emit lightChange();
	}

	
}
void light3D::keyPressEvent(QKeyEvent *e)
{
	/*switch(e ->key())
	{
	case Qt::Key_Up:
		sphereModelViewStack.RotateToWorld(0.8f,-1.0f,0.0f,0.0f);
		break;
	case Qt::Key_Down:
        sphereModelViewStack.RotateToWorld(-0.8f,-1.0f,0.0f,0.0f);
		break;
	case Qt::Key_Left:
		sphereModelViewStack.RotateToWorld(0.8f,0.0f,-1.0f,0.0f);
		break;
	case Qt::Key_Right:
		sphereModelViewStack.RotateToWorld(-0.8f,0.0f,-1.0f,0.0f);
		break;
	}
	
	updateGL();*/

}
void light3D::mouseMoveEvent(QMouseEvent *e)
{
	int deltax =  mouseOldX - e->x();
	int deltay =  mouseOldY - e->y();

	float yangle = 6.0*deltax/(this -> width());
    float xangle = 6.0*deltay/(this -> height());
	//////////////Ã»ÓĞrotatetoworld/////////////
	sphereModelViewStack.RotateToWorld(xangle,-1.0f,0.0f,0.0f);
	sphereModelViewStack.RotateToWorld(yangle,0.0f,-1.0f,0.0f);

	//sphereModelViewStack.Rotate(xangle, -1.0f, 0.0f, 0.0f);
	//sphereModelViewStack.Rotate(yangle, 0.0f, -1.0f, 0.0f);
	
    update();
	

	
}
void light3D::mousePressEvent(QMouseEvent *e)
{
	mouseOldX = e->x();
	mouseOldY = e->y();
}
void light3D::autoRotate()
{
	this ->modelViewStack.RotateToWorld(1.0,0.0f,1.0f,0.0f);
	//this->modelViewStack.Rotate(1.0, 0.0f, 1.0f, 0.0f);
    this ->update();
}