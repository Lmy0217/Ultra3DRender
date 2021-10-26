#pragma once
#ifndef LIGHT3D_H
#define LIGHT3D_H

//opengl lib
//#pragma comment(lib,"glew32.lib")
//#pragma comment(lib,"glew32s.lib")
//#pragma comment(lib,"gltools.lib")
//#pragma comment(lib,"freeglut.lib")
//#pragma comment(lib,"glut.lib")
//#pragma comment(lib,"glut32.lib")

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
//Qt lib
#include <QtWidgets/QOpenglWidget>
//#include <qopenglext.h>
//#include <qopenglwidget.h>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>
#include <QtCore/QVector>


class light3D:public /*QOpenGLWidget*/QGLWidget
{

	Q_OBJECT
public:
	light3D(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~light3D();

	void initializeGL();
	void paintGL();
	void resizeGL(int width,int height);
	void setAutoRotation(const bool _notifChange){this ->notifChange = _notifChange;}
	void resetRotaMatrix(){this ->modelViewStack.LoadIdentity();}
public slots:
	void autoRotate();

private:
	bool notifChange;


public:
	float ambientIn;
	void setAmbientIn(const float& _am){this ->ambientIn = _am;}
	const float getAmbinetIn() const{return this ->ambientIn;}

	float diffuseIn;
	void setDiffuseIn(const float& _dif){this ->diffuseIn = _dif;}
	const float getDiffuseIn() const{return this ->diffuseIn;}

	float specularIn;
	void setSpecularIn(const float& _spe){this ->specularIn = _spe;}
	const float getSpecularIn() const{return this ->specularIn;}

	float distance;
	void setDistance(const float& _dis){this ->distance = _dis;}
	const float getDistance() const{return this ->distance;}

	M3DVector3f ambient;
	const float* getAmbient(){return this ->ambient;}

	M3DVector3f diffuse;
	const float* getDiffuse(){return this ->diffuse;}

	M3DVector3f specular;
	const float* getSpecular(){return this ->specular;}

	GLint Ambient;
	GLint Diffuse;
	GLint Specular;
	GLint Distance;

	GLint AmbientI;
	GLint DiffuseI;
	GLint SpecularI;

    M3DVector3f position;
	const float* getPosition() const{return this ->position;}



	GLFrustum projection;
	GLMatrixStack projectionStack;
	GLMatrixStack sphereModelViewStack;
	GLMatrixStack modelViewStack;
    GLGeometryTransform spherePipline;
	GLGeometryTransform cubePipline;

	GLTriangleBatch sphereBatch;
	GLBatch cubeBatch;


protected:

	GLuint sphereShader;
	GLuint cubeShader;

	GLint sphereMVP;
	GLint sphereMV;
	GLint normalMV;
	GLint cubeMVP;
	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);

	int mouseOldX;
	int mouseOldY;



signals:
	void lightChange();
	void cudalightRotate(float xangle,float yangle);

};

#endif