
#ifndef SHADEWIDGET_H
#define SHADEWIDGET_H

#include <QtGui/QtGui>
#include "hoverpoints.h"
class ShadeWidget : public QWidget
{
    Q_OBJECT
public:
    enum ShadeType {
        RedShade,
        GreenShade,
        BlueShade,
        ARGBShade
    };

    ShadeWidget(ShadeType type, QWidget *parent);

    void setGradientStops(const QGradientStops &stops);

    void paintEvent(QPaintEvent *e);

    QSize sizeHint() const { return QSize(150, 40); }
    QPolygonF points() const;
	QGradientStops colors() const;

    HoverPoints* hoverPoints() const { return m_hoverPoints; }

	QPainterPath countPath;
	//QPolygonF countPoints;



//signals:
//    void colorsChanged(const QPolygonF& points);

private:
    void generateShade();

    ShadeType m_shade_type;
    
    HoverPoints *m_hoverPoints;
    QLinearGradient m_alpha_gradient;
public:
	QImage m_shade;
};



#endif 