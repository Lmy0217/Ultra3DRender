
#include "shadewidget.h"

ShadeWidget::ShadeWidget(ShadeType type, QWidget *parent)
    : QWidget(parent), m_shade_type(type), m_alpha_gradient(QLinearGradient(0, 0, 0, 0))
{

    // Checkers background
    if (m_shade_type == ARGBShade) {
		int w = 20;
	    int h = 20;
		int _w = w/2;
		int _h = h/2;
        QPixmap pm(w,h);
        QPainter pmp(&pm);
        pmp.fillRect(0, 0, _w, _h, Qt::lightGray);
        pmp.fillRect(_w, _h, _w, _h, Qt::lightGray);
        pmp.fillRect(0, _h, _w, _h, Qt::darkGray);
        pmp.fillRect(_w, 0, _w, _h, Qt::darkGray);
        pmp.end();
        QPalette pal = palette();
        pal.setBrush(backgroundRole(), QBrush(pm));
        setAutoFillBackground(true);
        setPalette(pal);

    } else {
        setAttribute(Qt::WA_NoBackground);

    }

    QPolygonF points;
   points << QPointF(0, sizeHint().height())
           << QPointF(sizeHint().width(), 0);


    m_hoverPoints = new HoverPoints(this, HoverPoints::CircleShape);


    m_hoverPoints->setPoints(points);
    m_hoverPoints->setPointLock(0, HoverPoints::LockToLeft);
    m_hoverPoints->setPointLock(1, HoverPoints::LockToRight);
    m_hoverPoints->setSortType(HoverPoints::XSort);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

}


QPolygonF ShadeWidget::points() const
{
    return m_hoverPoints->points();
}
QGradientStops ShadeWidget::colors() const
{
	return m_hoverPoints ->m_colors;
}


void ShadeWidget::setGradientStops(const QGradientStops &stops)
{
    if (m_shade_type == ARGBShade) {
        m_alpha_gradient = QLinearGradient(0, 0, width(), 0);
		m_alpha_gradient.setColorAt(0, QColor(0, 0, 0, 255));
        m_alpha_gradient.setColorAt(1, QColor(0, 0, 0, 255));
        for (int i = 0; i < stops.size(); i++) {
            QColor c = stops.at(i).second;
			
            m_alpha_gradient.setColorAt(stops.at(i).first, QColor(c.red(), c.green(), c.blue()));
			
        } 
	generateShade();
    update();
    }
}


void ShadeWidget::paintEvent(QPaintEvent *)
{
   

    QPainter p(this);
    p.drawImage(0, 0, m_shade);
	QBrush b = QBrush(QColor(255, 255, 255, 80));
    p.fillPath(this ->countPath,b);

  
}


void ShadeWidget::generateShade()
{

        if (m_shade_type == ARGBShade) {
            m_shade = QImage(size(), QImage::Format_ARGB32_Premultiplied);
            m_shade.fill(0);

            QPainter p(&m_shade);
            p.fillRect(rect(), m_alpha_gradient);

            p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            QLinearGradient fade(0, 0, 0, height());
            fade.setColorAt(0, QColor(0, 0, 0, 0));
			fade.setColorAt(0.95, QColor(0, 0, 0, 255));
    
            p.fillRect(rect(), fade);

    }


}
