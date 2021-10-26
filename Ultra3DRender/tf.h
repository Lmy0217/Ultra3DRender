#ifndef TF_H
#define TF_H

#include <QtWidgets/QWidget>
#include <QtGui/QIcon>
#include "shadewidget.h"
const int fixed_w = 1024;
const int fixed_h = 256;
class TF : public QWidget
{
	Q_OBJECT

public:
	TF(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~TF();

	void resizeEvent( QResizeEvent * event );
	void closeEvent(QCloseEvent *event);
	

	unsigned char* tfData;
	unsigned char* getTFData() const
	{
		return this ->tfData;
	}
	//for the number hist..
	unsigned int* count;
	unsigned int countSize;
	unsigned int mostHigh ;
	
	//set count data
	void setCountSizeAndMostHight(const unsigned int& Size,const unsigned int &High)
	{
		this ->countSize = Size;
		this ->mostHigh = High;
	}
	void setCountData(unsigned int* data);
	//save and load gradientStop,points;
	void saveData(const QString& str);
	void loadData(const QString& str);
private:

    ShadeWidget*  m_alpha_shade;
	bool loadFromFile;
    //draw count data
	void drawCountData();
public slots:
	void pointsUpdated();


signals:
	void tfChange();
};

#endif // TF_H
