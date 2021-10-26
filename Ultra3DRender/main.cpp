#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QElapsedTimer>

#include "v3d.h"


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	//QSplashScreen *splash = new QSplashScreen;
 //   splash->setPixmap(QPixmap(":/Resources/screen logo/screen.png"));
 //   splash->show();
	a.processEvents();

	//middle
	QDesktopWidget* pDesk = QApplication::desktop();

	V3D w;
	w.move((pDesk->width() - w.width()) / 2, (pDesk->height() - w.height()) / 2);

	QFont font = a.font();
	font.setFamily("msyh");
	font.setPixelSize(12);
	a.setFont(font);

	w.show();

	//splash->finish(&w);
	//delete splash;

	return a.exec();
}
