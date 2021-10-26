#include "v3d.h"

V3D::V3D(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent)
{
	
	QIcon icon;
    icon.addFile(QString::fromUtf8(":/Resources/images.jpg"), QSize(), QIcon::Normal, QIcon::On);
	this ->setWindowIcon(icon);
	ui.setupUi(this);

    this ->autoRotaTimer = new QTimer();

	QWidget *mainSettingTitleBar1 = new QWidget(this);
	this ->mainSetting = new QDockWidget(this);
	this ->mainSetting ->setFeatures(QDockWidget::DockWidgetMovable);
	this ->mainSetting ->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);  
	this ->mainSetting ->setWidget(ui.mainSettingsDesk);
	this ->mainSetting ->setTitleBarWidget(mainSettingTitleBar1);
	this ->addDockWidget(Qt::RightDockWidgetArea,this ->mainSetting); 
	//set Action
	this ->addActions();

	this ->basicToolBar = new QToolBar(tr("&Basic Tool"));
	this ->basicToolBar ->addAction(this ->openUltraFileAction);
	this ->basicToolBar ->addAction(this ->openCTFileAction);
	this ->basicToolBar ->addAction(this ->saveImage);
	this ->addToolBar(Qt::BottomToolBarArea,this ->basicToolBar);

	this ->tfToolBar = new QToolBar(tr("&TF Tool"));
	this ->tfToolBar ->addAction(this ->displayTF);
	this ->tfToolBar ->addAction(this ->importTF);
	this ->tfToolBar ->addAction(this ->exportTF);
	this ->tfToolBar ->addWidget(ui.useTFColors);
	this ->addToolBar(Qt::BottomToolBarArea,this ->tfToolBar);

	
	this ->displayToolBar = new QToolBar(tr("&Display Tool"));
	this ->displayToolBar ->addAction(this ->zoomIn);
	this ->displayToolBar ->addAction(this ->zoomOut);
	this ->displayToolBar ->addAction(this ->setBGColor);
	this ->displayToolBar ->addWidget(ui.HDRcheckBox);
	this ->displayToolBar ->addWidget(ui.autoRotate);
	this ->addToolBar(Qt::BottomToolBarArea,this ->displayToolBar);

    //temp of light choosen
	this ->lightStyle = indirectPhongIllum;
	this->fileName = "volume.raw";
	this ->ultr3d = new ultr3D(this ->fileName,this);
	
	//this ->ultr3d ->show();
    this ->tf = new TF();
	this ->tf ->loadData(this ->fileName);
 

    /*QWidget *mainSettingTitleBar2 = new QWidget(this);
	this ->tfSetting = new QDockWidget(this);
	this ->tfSetting ->setFeatures(QDockWidget::DockWidgetMovable);
	this ->tfSetting ->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);  
	this ->tfSetting ->setWidget(this ->tf);
	this ->tfSetting ->setTitleBarWidget(mainSettingTitleBar2);
	this ->addDockWidget(Qt::TopDockWidgetArea,this ->tfSetting); */

    this ->initializeUltr3dAndTF();
    this ->tf ->show();

	
	

	//signal connect slot
	//////////////////////////////////////////////////////////////////////////////

	QObject::connect(this ->autoRotaTimer,SIGNAL(timeout()),ui.light3d,SLOT(autoRotate()));
	QObject::connect(this ->tf,SIGNAL(tfChange()),this,SLOT(setTFTexture()));
	QObject::connect(ui.ambientButton,SIGNAL(clicked()),this,SLOT(openAmbientC()));
	QObject::connect(ui.diffuseButton,SIGNAL(clicked()),this,SLOT(openDiffuseC()));
	QObject::connect(ui.specularButton,SIGNAL(clicked()),this,SLOT(openSpecularC()));
	

	QObject::connect(&(this ->AmbientC),SIGNAL(currentColorChanged(QColor)),this,SLOT(setAmbientC(QColor)));
	QObject::connect(&(this ->DiffuseC),SIGNAL(currentColorChanged(QColor)),this,SLOT(setDiffuseC(QColor)));
	QObject::connect(&(this ->SpecularC),SIGNAL(currentColorChanged(QColor)),this,SLOT(setSpecularC(QColor)));
	QObject::connect(&(this ->backC),SIGNAL(currentColorChanged(QColor)),this,SLOT(setBackC(QColor)));

	QObject::connect(ui.ambientSlider,SIGNAL(valueChanged(int)),this,SLOT(setAmbientI(int)));
	QObject::connect(ui.diffuseSlider,SIGNAL(valueChanged(int)),this,SLOT(setDiffuseI(int)));
	QObject::connect(ui.specularSlider,SIGNAL(valueChanged(int)),this,SLOT(setSpecularI(int)));
	QObject::connect(ui.distanceSlider,SIGNAL(valueChanged(int)),this,SLOT(setDistance(int)));

	

	QObject::connect(ui.localLightCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));
	QObject::connect(ui.noLightCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));
	QObject::connect(ui.directIlluminationCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));
	QObject::connect(ui.indirectIlluminationCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));
	QObject::connect(ui.directPhongIlluminationCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));
	QObject::connect(ui.globalIlluminationCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));
	QObject::connect(ui.DirectIndirectPhongCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));
	QObject::connect(ui.inDirectPhongCheck,SIGNAL(clicked()),this,SLOT(lightTurn()));

	QObject::connect(ui.samplingCount,SIGNAL(valueChanged(int)),this,SLOT(setSamplingCount(int)));
	QObject::connect(ui.gradientCount,SIGNAL(valueChanged(int)),this,SLOT(setGradientCount(int)));
	
	QObject::connect(ui.surfaceShadingParameter,SIGNAL(valueChanged(int)),this ,SLOT(setSSP(int)));
	//HDR
	QObject::connect(ui.exposureSlider,SIGNAL(valueChanged(int)),this,SLOT(setHDRExposure(int)));
	//gamma corrction
	QObject::connect(ui.gammaCorrectionSlider,SIGNAL(valueChanged(int)),this,SLOT(setGammaCorrection(int)));
	//light attenuation
	QObject::connect(ui.lightAttenuationSlider,SIGNAL(valueChanged(int)),this,SLOT(setLightAttenuation(int)));
	QObject::connect(ui.scatteringCoeffSlider,SIGNAL(valueChanged(int)),this,SLOT(setScatteringCoeff(int)));
	//HDR on
	QObject::connect(ui.HDRcheckBox,SIGNAL(stateChanged(int)),this,SLOT(setHDRon(int)));
	//LIGHT AND CUDA LIGHT STACK
	QObject::connect(ui.light3d,SIGNAL(lightChange()),this,SLOT(setLightProp()));
	//QObject::connect(ui.light3d,SIGNAL(cudalightRotate(float,float)),this,SLOT(cudalightGet(float,float)));
	//color box
	QObject::connect(ui.colorBoxSlider,SIGNAL(valueChanged(int)),this,SLOT(setColorBox(int)));	
	//if using tf colors
	QObject::connect(ui.useTFColors,SIGNAL(stateChanged(int)),this,SLOT(setUseTFColors(int)));
	QObject::connect(ui.autoRotate,SIGNAL(stateChanged(int)),this,SLOT(setAutoRotate(int)));

	
	
}

V3D::~V3D()
{

	if(NULL != this ->ultr3d)
		delete(this ->ultr3d);
	if(NULL != this ->tf)
		delete(this ->tf);
	if(NULL != this ->autoRotaTimer)
		delete(this ->autoRotaTimer);
}

void V3D::keyPressEvent(QKeyEvent *e)
{

	//switch(e ->key())
	//{
	//case Qt::Key_Up:
	//	(this ->ultr3d)->modelviewStack.RotateToWorld(10.0f,-1.0f,0.0f,0.0f);
	//	break;
	//case Qt::Key_Down:
    //  (this ->ultr3d)->modelviewStack.RotateToWorld(-10.0f,-1.0f,0.0f,0.0f);
	//	break;
	//case Qt::Key_Left:
	//	(this ->ultr3d)->modelviewStack.RotateToWorld(10.0f,0.0f,-1.0f,0.0f);
	//	break;
	//case Qt::Key_Right:
	//	(this ->ultr3d)->modelviewStack.RotateToWorld(-10.0f,0.0f,-1.0f,0.0f);
	//	break;
	//}
	//(this ->ultr3d)->LPChange = true;    
	//(this ->ultr3d)->update();


}
void V3D::closeEvent(QCloseEvent *event)
{
	exit(0);
}


void V3D::setTFTexture()
{
	memcpy(this ->ultr3d ->getTFData(),this ->tf ->getTFData(),256 * 4);
	this ->ultr3d ->loadTFTex();
	this ->ultr3d ->update();
}


void V3D::setAmbientI(int t)
{
	
	ui.light3d ->setAmbientIn(t/100.0);
	ui.ambiemtIL ->setText( "Ambient Instenity " + QString::number(ui.light3d ->ambientIn));
	ui.light3d ->update();
	
}
void V3D::openAmbientC()
{
	AmbientC.open();
}
void V3D::setAmbientC(QColor c)
{
	m3dLoadVector3(ui.light3d ->ambient,c.red()/255.0,c.green()/255.0,c.blue()/255.0);
	QString Color = "rgb(";
	Color = Color + QString::number(c.red()) + "," + QString::number(c.green()) + "," +QString::number(c.blue()) + ");";
	ui.ambientButton ->setStyleSheet("background-color: " + Color);
	ui.light3d ->update();
	//qDebug() << "    a "<<ui.light ->ambient[1];

}
void V3D::setDiffuseI(int t)
{
	ui.light3d ->setDiffuseIn(t/100.0);
	ui.diffuseIL ->setText( "Diffuse Instenity " + QString::number(ui.light3d ->diffuseIn));
	ui.light3d ->update();

}
void V3D::openDiffuseC()
{
	DiffuseC.open();
}
void V3D::setDiffuseC(QColor c)
{
	m3dLoadVector3(ui.light3d ->diffuse,c.red()/255.0,c.green()/255.0,c.blue()/255.0);
	QString Color = "rgb(";
	Color = Color + QString::number(c.red()) + "," + QString::number(c.green()) + "," +QString::number(c.blue()) + ");";
	ui.diffuseButton ->setStyleSheet("background-color: " + Color);
	ui.light3d ->update();

}
void V3D::setSpecularI(int t)
{
	ui.light3d ->setSpecularIn(t/100.0);
	ui.specularIL ->setText( "Specular Instenity " + QString::number(ui.light3d ->specularIn));
	ui.light3d ->update();
	

}
void V3D::openSpecularC()
{
	SpecularC.open();
}
void V3D::setSpecularC(QColor c)
{
	m3dLoadVector3(ui.light3d ->specular,c.red()/255.0,c.green()/255.0,c.blue()/255.0);
	QString Color = "rgb(";
	Color = Color + QString::number(c.red()) + "," + QString::number(c.green()) + "," +QString::number(c.blue()) + ");";
	ui.specularButton ->setStyleSheet("background-color: " + Color);
	ui.light3d ->update();


}
void V3D::openBackC()
{
	this ->backC.open();
	//this->ultr3d->modelviewStack.LoadIdentity();
	//this->ultr3d->update();
}
void V3D::setBackC(QColor c)
{
	this ->ultr3d ->setBackColor(c.red()/255.0,c.green()/255.0,c.blue()/255.0);
	this ->ultr3d ->update();
}
void V3D::setDistance(int t)
{
	ui.light3d ->setDistance(t/10.0);
	ui.distanceL ->setText( "Distance " + QString::number(ui.light3d ->distance));
	ui.light3d ->update();

}
void V3D::rotateLightModel(float xangle,float yangle,float zangle)
{
	//////////////////////////////////////////////
	ui.light3d ->modelViewStack.RotateToWorld(xangle,-1.0f,0.0f,0.0f);
	ui.light3d ->modelViewStack.RotateToWorld(yangle,0.0f,-1.0f,0.0f);
	ui.light3d ->modelViewStack.RotateToWorld(zangle,0.0f,1.0f,0.0f);

	//ui.light3d->modelViewStack.Rotate(xangle, -1.0f, 0.0f, 0.0f);
	//ui.light3d->modelViewStack.Rotate(yangle, 0.0f, -1.0f, 0.0f);
	//ui.light3d->modelViewStack.Rotate(zangle, 0.0f, 1.0f, 0.0f);
	//ui.light3d->modelViewStack.RotateToWorld(yangle, 1.0f, 0.0f, 0.0f);
	//ui.light3d->modelViewStack.RotateToWorld(xangle, 0.0f, 1.0f, 0.0f);
	//ui.light3d->modelViewStack.RotateToWorld(zangle, 0.0f, 0.0f, 1.0f);
    ui.light3d ->update();

}

void V3D::setLightProp()
{

	M3DVector3f ambient;
	M3DVector3f diffuse;
	M3DVector3f specular;
	M3DVector4f lightPosition;
	M3DVector4f _lightPosition;
	M3DVector3f temp;

	m3dCopyVector3(ambient,ui.light3d ->getAmbient());//设置的颜色/255
	m3dCopyVector3(diffuse,ui.light3d ->getDiffuse());
	m3dCopyVector3(specular,ui.light3d ->getSpecular());
	m3dCopyVector3(lightPosition,ui.light3d ->getPosition());
	lightPosition[3] = 0.0f;
	
	m3dScaleVector3(ambient,ui.light3d ->getAmbinetIn());//设置的参数*设置的颜色/255，得到颜色系数.eg(0.4,0.3,0.5);
	m3dScaleVector3(diffuse,ui.light3d ->getDiffuseIn());
	m3dScaleVector3(specular,ui.light3d ->getSpecularIn());

	m3dTransformVector4(_lightPosition,lightPosition,ui.light3d ->sphereModelViewStack.GetMatrix());
	m3dCopyMatrix44(this ->ultr3d ->lightModelViewMatrix,ui.light3d ->sphereModelViewStack.GetMatrix());

	temp[0] = _lightPosition[0];
	temp[1] = _lightPosition[1];
	temp[2] = _lightPosition[2];
	

	m3dNormalizeVector3(temp);
	m3dScaleVector3(temp,ui.light3d ->distance);

	m3dLoadVector3(this ->ultr3d ->ambient,ambient[0],ambient[1],ambient[2]); 
    m3dLoadVector3(this ->ultr3d ->diffuse,diffuse[0],diffuse[1],diffuse[2]); 
	m3dLoadVector3(this ->ultr3d ->specular,specular[0],specular[1],specular[2]); 
	m3dLoadVector4(this ->ultr3d ->lightPositionVec4,temp[0],temp[1],temp[2],0.0f);  
	
	//m3dLoadVector4(this ->ultr3d ->lightPositionFixedVec4,0.0f,0.0f,1.0f,0.0f);
	//m3dScaleVector4(this ->ultr3d ->lightPositionFixedVec4,ui.light3d ->distance);

	this ->ultr3d ->isLPChange(true);
	this ->ultr3d ->update();


}
void V3D::lightTurn()
{
	if(ui.localLightCheck ->isChecked())
		this ->ultr3d ->light = localLight;
	else if(ui.noLightCheck ->isChecked())
		this ->ultr3d ->light = noLight;
	else if(ui.directIlluminationCheck ->isChecked())
		this ->ultr3d ->light = directLight;
	else if(ui.indirectIlluminationCheck ->isChecked())
		this ->ultr3d ->light = indrectLight;
	else if(ui.globalIlluminationCheck ->isChecked())
		this ->ultr3d ->light = globalLight;
	else if(ui.directPhongIlluminationCheck ->isChecked())
		this ->ultr3d ->light = directPhongIllum;
	else if(ui.DirectIndirectPhongCheck ->isChecked())
		this ->ultr3d ->light = globalPhongIllum;
	else if(ui.inDirectPhongCheck ->isChecked())
		this ->ultr3d ->light = indirectPhongIllum;

	this ->lightStyle = this ->ultr3d ->light;
	this ->ultr3d ->update();

}
void V3D::setSamplingCount(int t)
{
	this ->ultr3d ->setSampleStep(t);
	ui.sampleCL ->setText( "Sample count " + QString::number(t));
	this ->ultr3d ->update();


}
void V3D::setGradientCount(int t)
{
	this ->ultr3d ->setGradientStep(t);
	ui.gradientCL ->setText( "Gradient count " + QString::number(t));
	this ->ultr3d ->update();


}

//set surface shading parameter
void V3D::setSSP(int t)
{
	this ->ultr3d ->setSurfaceShadingParameter(t/100.0);
	ui.SSPL ->setText("Surface Shading Parameter " +QString::number(t/100.0));
	this ->ultr3d ->update();
}
//set HDR
void V3D::setHDRExposure(int t)
{
	this ->ultr3d ->setExposure(t /10.0);
	ui.exposureL ->setText("HDR Exposure " + QString::number(t/10.0));
	this ->ultr3d ->update();
}

//set gamma correction
void V3D::setGammaCorrection(int t)
{
	this ->ultr3d ->setGammaCorrection(t/10.0);
	ui.gammaCorrectionL ->setText("Gamma Correction " + QString::number(t/10.0));
	this ->ultr3d ->update();
}
//set light attenuation
void V3D::setLightAttenuation(int t)
{
	this ->ultr3d ->setKl(t/10.0);
	ui.lightAttenuationL ->setText("Light Attenuation " + QString::number(t/10.0));
	this ->ultr3d ->update();
}
//set colorBox
void V3D::setColorBox(int t)
{
	this ->ultr3d ->setColorBox((t - 100)/100.0);
	ui.colorBoxL ->setText(QString::number((t - 100)/100.0));
	this ->ultr3d ->update();
}
void V3D::setScatteringCoeff(int t)
{
	/*this ->ultr3d ->scatteringMost = t /255.0;
	ui.scatteringCoeffLabel ->setText("Scattering Coeff " + QString::number(t / 255.0));
	this ->ultr3d ->cudaScatterChange();
	this ->ultr3d ->update();*/
}
//set HDR on
void V3D::setHDRon(int t)
{
	if(Qt::Unchecked == t)
		this ->ultr3d ->isHdrOn(false);
	if(Qt::Checked == t)
		this ->ultr3d ->isHdrOn(true);
	this ->ultr3d ->update();
}

//set ultr3d,tf visable
void V3D::setUltr3dVis()
{
	this ->ultr3d ->setVisible(true);
}
void V3D::setTFVis()
{
	this ->tf ->setVisible(true);

}
//read Data
void V3D::loadUltrData() throw(V3dException)
{
	
	QString _fileName = this ->getFileName("raw");
	if("" != _fileName)
	{
		this ->fileName = _fileName;
		if(NULL != this ->ultr3d)
		{	
			delete(this ->ultr3d);
		}
		this ->ultr3d = new ultr3D(this ->fileName,this);
		this ->initializeUltr3dAndTF();
		this ->ultr3d ->show();
	}
}
void V3D::loadCTData()
{
	QString _fileName = this ->getFileName("raw");
	if("" != _fileName)
	{
		this ->fileName = _fileName;
		//read .txt to width,height,depth
		QString domain = _fileName;
		domain.replace(".raw",".txt");
        std::string d = domain.toUtf8().constData();//.toStdString();
		
		FILE *fp;
		fp = fopen(d.c_str(),"r");
	    if(NULL == fp)
	    {
			QMessageBox::information(NULL, "warning", "please note the width height depth in .txt in the same file with same name at .raw", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			return;
	     }

		int width = 0,height = 0,depth  = 0;
		fscanf(fp,"%d",&width);
		fscanf(fp,"%d",&height);
		fscanf(fp,"%d",&depth);
		fclose(fp);
	    fp = NULL;
		
		if(NULL != this ->ultr3d)
		{	
			delete(this ->ultr3d);
		}
		this ->ultr3d = new ultr3D(this ->fileName,width,height,depth,this);
		this ->initializeUltr3dAndTF();
		this ->ultr3d  ->show();
		

	}
}
QString V3D::getFileName(const QString _fileStyle) const
{
    QString fileName = "";
	fileName = QFileDialog::getOpenFileName(NULL,tr("Open Volume"),NULL,("Volumes(*." + _fileStyle +")"));
    qDebug()<<fileName;
	if("" == this ->fileName)
	{
		QMessageBox::information(NULL, "warning", "open file cann't be null", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return "";
	}else
	{
		return fileName;
	}

}
//initial another ultr3D again
void V3D::initializeUltr3dAndTF()
{	
	//initial the count of tf////////////////////////
	this ->tf ->setCountSizeAndMostHight(this ->ultr3d ->mostHighX,this ->ultr3d ->mostHigh);
	this ->tf ->setCountData(this ->ultr3d ->grayCount);
    this ->tf ->resizeEvent(NULL); 
	/////////////////////////////////////////////////
    QObject::connect(this ->ultr3d,SIGNAL(sendFPS(unsigned int)),this,SLOT(setFPS(unsigned int)));
	QObject::connect(this ->ultr3d,SIGNAL(rotate(float,float,float)),this,SLOT(rotateLightModel(float,float,float)));
    QObject::connect(this ->autoRotaTimer,SIGNAL(timeout()),this ->ultr3d,SLOT(autoRotate()));
	this ->ultr3d ->setSampleStep(ui.samplingCount ->value());
	this ->ultr3d ->setGradientStep(ui.gradientCount ->value());
	this ->ultr3d ->setSurfaceShadingParameter(ui.surfaceShadingParameter ->value()/100.0);
	this ->ultr3d ->setExposure(ui.exposureSlider ->value() /10.0);
	this ->ultr3d ->gammaCorrection = ui.gammaCorrectionSlider ->value()/10.0;
	this ->ultr3d ->setKl(ui.lightAttenuationSlider ->value()/10.0);
	this ->ultr3d ->setColorBox((ui.colorBoxSlider ->value() - 100)/100.0);
	if(Qt::Unchecked == ui.HDRcheckBox->checkState())
		this ->ultr3d ->isHdrOn(false);
	if(Qt::Checked == ui.HDRcheckBox->checkState())
		this ->ultr3d ->isHdrOn(true);
	if(Qt::Unchecked == ui.useTFColors ->checkState())
		this ->ultr3d ->isUseTFColors(false);
	if(Qt::Checked == ui.useTFColors ->checkState())
		this ->ultr3d ->isUseTFColors(true);
	
	this ->ultr3d ->light = this ->lightStyle;
	this ->setTFTexture();
	this ->setLightProp();

	this ->setCentralWidget(this ->ultr3d);
	this ->ultr3d ->update();
	
		
	ui.light3d ->resetRotaMatrix();
	ui.light3d ->update();

}
//save tf data and load tf data
void V3D::saveTF()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("hh_mm_ss");
	QString tempfile = this ->fileName;
	tempfile.replace(".raw",current_date);
	QString str = tempfile + ".tf";
	qDebug() << str;
	this ->tf ->saveData(str);
	

}
void V3D::loadTF()
{
	QString str = this ->getFileName("tf");
	if("" != str)
	{
		str.replace('/','\\');
		this ->tf ->loadData(str);
		
	}

}
//set FPS
void V3D::setFPS(unsigned int fps)
{
	this ->setWindowTitle(QString::fromUtf8("3D Ultrasound Visualization Platform ") + QString::number((1000000000.0/fps),'f',2) + " fps");
	//ui.light3d->update();
}

//set if using TF colors
void V3D::setUseTFColors(int t)
{

	if(Qt::Unchecked == t)
		this ->ultr3d ->isUseTFColors(false);
	if(Qt::Checked == t)
		this ->ultr3d ->isUseTFColors(true);
	this ->ultr3d ->update();

}
//save Image Button
void V3D::saveImageButton()
{
	this ->ultr3d ->update();
	this ->ultr3d ->saveImage();
}
//zoom in and out
void V3D::zoomResIn()
{
	this ->ultr3d ->zoom(1.01f);
	this ->ultr3d ->update();
}
void V3D::zoomResOut()
{
	this ->ultr3d ->zoom(1.0/1.01f);
	this ->ultr3d ->update();
}
//auto Rotate
void V3D::setAutoRotate(int t)
{
	if(Qt::Checked == t)
	{
		this ->autoRotaTimer ->start(10);
		ui.light3d ->setAutoRotation(false);
		
	}
	if(Qt::Unchecked == t)
	{
		this ->autoRotaTimer ->stop();
		ui.light3d ->setAutoRotation(true);
		ui.light3d ->update();
	}
   
}


//add toolBar
void V3D::addActions()
{
	//openfile
	this ->openUltraFileAction = new QAction(tr("&Open a Ultrasound data file."),this);
	this ->openUltraFileAction ->setIcon(QIcon(TOOLBARRS + "fileopen.png"));
	QObject::connect(this ->openUltraFileAction,SIGNAL(triggered()),this,SLOT(loadUltrData()));
	
	//openfile
	this ->openCTFileAction = new QAction(tr("&Open a CT data file."),this);
	this->openCTFileAction->setIcon(QIcon(TOOLBARRS + "ctfileopen.png"));
	QObject::connect(this ->openCTFileAction,SIGNAL(triggered()),this,SLOT(loadCTData()));

	//saveImage
	this ->saveImage = new QAction(tr("&Save a screen shot."),this);
	this ->saveImage ->setIcon(QIcon(TOOLBARRS + "imagesave.png"));
	QObject::connect(this ->saveImage,SIGNAL(triggered()),this,SLOT(saveImageButton()));

	//display tf
	this ->displayTF = new QAction(tr("&Display Transform Function."),this);
	this ->displayTF ->setIcon(QIcon(TOOLBARRS + "tfdisplay.png"));
	QObject::connect(this ->displayTF,SIGNAL(triggered()),this,SLOT(setTFVis()));
	//import tf
	this ->importTF = new QAction(tr("&Import Transform Function."),this);
	this ->importTF ->setIcon(QIcon(TOOLBARRS + "tfimport.png"));
	QObject::connect(this ->importTF,SIGNAL(triggered()),this,SLOT(loadTF()));

	//export tf
	this ->exportTF = new QAction(tr("&Export Transform Function"),this);
	this ->exportTF ->setIcon(QIcon(TOOLBARRS + "tfexport.png"));
	QObject::connect(this ->exportTF,SIGNAL(triggered()),this,SLOT(saveTF()));

	//Zoom in
	this ->zoomIn = new QAction(tr("&Zoom in|right click + up"), this);
	this ->zoomIn ->setIcon(QIcon(TOOLBARRS + "zoomin.png"));
	QObject::connect(this ->zoomIn,SIGNAL(triggered()),this,SLOT(zoomResIn()));

	//Zoom out
	this ->zoomOut = new QAction(tr("&Zoom out|right click + down"),this);
	this ->zoomOut ->setIcon(QIcon(TOOLBARRS + "zoomout.png"));
	QObject::connect(this ->zoomOut,SIGNAL(triggered()),this,SLOT(zoomResOut()));

	//set background color
	this ->setBGColor = new QAction(tr("&Set the painting background color."),this);
	this ->setBGColor ->setIcon(QIcon(TOOLBARRS + "bgcolorset.png"));
	QObject::connect(this ->setBGColor,SIGNAL(triggered()),this,SLOT(openBackC()));
}