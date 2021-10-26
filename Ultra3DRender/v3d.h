#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore/QString>
#include <QtWidgets/QColorDialog>
#include <QtGui/QColor>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <math3d.h>
#include "ui_v3d.h"
#include "ultr3d.h"
#include "tf.h"
#include "light3d.h"

using namespace std;
//enum LightStyle {noLight = 0, localLight, directLight, indrectLight, globalLight, directPhongIllum, globalPhongIllum, indirectPhongIllum }
const QString TOOLBARRS = ":/Resources/toolbar logo/";
const QString SCREENLOGOS = ":/Resources/screen logo/screen";
class V3D : public QMainWindow
{
	Q_OBJECT

public:
	V3D(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~V3D();
	void keyPressEvent(QKeyEvent* e);
	void closeEvent(QCloseEvent* event);



private:

	void addActions();

	//Action
	QAction* openUltraFileAction;
	QAction* openCTFileAction;
	QAction* saveImage;
	QAction* displayTF;
	QAction* importTF;
	QAction* exportTF;
	QAction* zoomIn;
	QAction* zoomOut;
	QAction* setBGColor;

	//ToolBar
	QToolBar* basicToolBar;
	QToolBar* tfToolBar;
	QToolBar* displayToolBar;



	//MainSetting 
	QDockWidget* mainSetting;
	//tfsetting
	QDockWidget* tfSetting;



	ultr3D* ultr3d;
	TF* tf;
	light3D* light3d;

	Ui::V3DClass ui;

	QColorDialog AmbientC;
	QColorDialog DiffuseC;
	QColorDialog SpecularC;

	QColorDialog backC;

	QString fileName;

	//the temp data of ultr3d
	int lightStyle;

	QTimer* autoRotaTimer;
private:
	QString getFileName(const QString _fileStyle) const;

public slots:
	void setTFTexture();

	void setAmbientI(int t);
	void openAmbientC();
	void setAmbientC(QColor c);
	void setDiffuseI(int t);
	void openDiffuseC();
	void setDiffuseC(QColor c);
	void setSpecularI(int t);
	void openSpecularC();
	void setSpecularC(QColor c);
	//set back Color
	void openBackC();
	void setBackC(QColor c);

	void setDistance(int t);
	void rotateLightModel(float xangle, float yangle, float zangle);

	void setLightProp();
	void lightTurn();

	void setSamplingCount(int t);
	void setGradientCount(int t);

	void setSSP(int t);
	void setHDRExposure(int t);
	void setGammaCorrection(int t);
	void setLightAttenuation(int t);
	void setColorBox(int);
	void setHDRon(int);
	void setScatteringCoeff(int t);
	//set ultr3d,tf visiable 
	void setUltr3dVis();
	void setTFVis();

	//read Data
	void loadUltrData();
	void loadCTData();

	//set FPS
	void setFPS(unsigned int fps);

	//CUDA
	//void cudalightGet(float xangle,float yangle);

	//initial ultr3d again
	void initializeUltr3dAndTF();

	//save tf data
	void saveTF();
	//load tf data
	void loadTF();
	//set use TF colors
	void setUseTFColors(int t);
	//save Image
	void saveImageButton();

	//zoom in and out
	void zoomResIn();
	void zoomResOut();
	//auto Rotate
	void setAutoRotate(int t);
	//void autoRotate();

};