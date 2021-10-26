
#include "hoverpoints.h"
#include <QtWidgets/QApplication>

#define printf

HoverPoints::HoverPoints(QWidget *widget, PointShape shape)
    : QObject(widget)
{
    m_widget = widget;
    widget->installEventFilter(this);
    widget->setAttribute(Qt::WA_AcceptTouchEvents);

    m_connectionType =  LineConnection;
    m_sortType = NoSort;
    m_shape = shape;
    m_pointPen = QPen(QColor(255, 255, 255, 191), 1);
    m_connectionPen = QPen(QColor(255, 255, 255, 140), 2);
    m_pointBrush = QBrush(QColor(191, 191, 191, 127));
    m_pointSize = QSize(15, 15);
    m_currentIndex = -1;
    m_editable = true;
    m_enabled = true;

	this ->is_new_point = false;
	QColor c(0,0,0);
    QGradientStop g(-1.0,c);
	this ->m_colors.insert(0,g);
	this ->m_colors.insert(1,g);

	QObject::connect(&(this ->color),SIGNAL(currentColorChanged(QColor)),this,SLOT(setColor(QColor)));


}


void HoverPoints::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        m_widget->update();
    }
}


bool HoverPoints::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_widget && m_enabled) {
        switch (event->type()) {

        case QEvent::MouseButtonPress:
        {
			
            if (!m_fingerPointMapping.isEmpty())
                return true;
            QMouseEvent *me = (QMouseEvent *) event;

            QPointF clickPos = me->pos();
            int index = -1;
            for (int i=0; i<m_points.size(); ++i) {
                QPainterPath path;
                if (m_shape == CircleShape)
                    path.addEllipse(pointBoundingRect(i));
                else
                    path.addRect(pointBoundingRect(i));

                if (path.contains(clickPos)) {
                    index = i;
				
                    break;
                }
            }

            if (me->button() == Qt::LeftButton) {
                if (index == -1) {
                    if (!m_editable)
                        return false;
                    int pos = 0;
                    // Insert sort for x or y
                    if (m_sortType == XSort) {
                        for (int i=0; i<m_points.size(); ++i)
                            if (m_points.at(i).x() > clickPos.x()) {
                                pos = i;
                                break;
                            }
                    } else if (m_sortType == YSort) {
                        for (int i=0; i<m_points.size(); ++i)
                            if (m_points.at(i).y() > clickPos.y()) {
                                pos = i;
                                break;
                            }
                    }

                    m_points.insert(pos, clickPos);

					this ->is_new_point = true;
					QColor c(0,0,0);
					QGradientStop g(-1.0,c);
					this ->m_colors.insert(pos,g);
			

                    m_locks.insert(pos, 0);
                    m_currentIndex = pos;
                    firePointChange();
                } else {
                    m_currentIndex = index;
                }
                return true;

            } else if (me->button() == Qt::RightButton) {
                if (index >= 0 && m_editable) {
                    if (m_locks[index] == 0) {
                        m_locks.remove(index);
                        m_points.remove(index);
						this ->m_colors.remove(index);
                    }
                    firePointChange();
                    return true;
                }
            }

        }
        break;

        case QEvent::MouseButtonRelease:
            if (!m_fingerPointMapping.isEmpty())
                return true;
            m_currentIndex = -1;
            break;

        case QEvent::MouseMove:
            if (!m_fingerPointMapping.isEmpty())
                return true;
            if (m_currentIndex >= 0)
                movePoint(m_currentIndex, ((QMouseEvent *)event)->pos());
            break;
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
            {
                const QTouchEvent *const touchEvent = static_cast<const QTouchEvent*>(event);
                const QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();
                const qreal pointSize = qMax(m_pointSize.width(), m_pointSize.height());
                foreach (const QTouchEvent::TouchPoint &touchPoint, points) {
                    const int id = touchPoint.id();
                    switch (touchPoint.state()) {
                    case Qt::TouchPointPressed:
                        {
                            // find the point, move it
                            QSet<int> activePoints = QSet<int>::fromList(m_fingerPointMapping.values());
                            int activePoint = -1;
                            qreal distance = -1;
                            const int pointsCount = m_points.size();
                            const int activePointCount = activePoints.size();
                            if (pointsCount == 2 && activePointCount == 1) { // only two points
                                activePoint = activePoints.contains(0) ? 1 : 0;
                            } else {
                                for (int i=0; i<pointsCount; ++i) {
                                    if (activePoints.contains(i))
                                        continue;

                                    qreal d = QLineF(touchPoint.pos(), m_points.at(i)).length();
                                    if ((distance < 0 && d < 12 * pointSize) || d < distance) {
                                        distance = d;
                                        activePoint = i;
                                    }

                                }
                            }
                            if (activePoint != -1) {
                                m_fingerPointMapping.insert(touchPoint.id(), activePoint);
                                movePoint(activePoint, touchPoint.pos());
                            }
                        }
                        break;
                    case Qt::TouchPointReleased:
                        {
                            // move the point and release
                            QHash<int,int>::iterator it = m_fingerPointMapping.find(id);
                            movePoint(it.value(), touchPoint.pos());
                            m_fingerPointMapping.erase(it);
                        }
                        break;
                    case Qt::TouchPointMoved:
                        {
                            // move the point
                            const int pointIdx = m_fingerPointMapping.value(id, -1);
                            if (pointIdx >= 0) // do we track this point?
                                movePoint(pointIdx, touchPoint.pos());
                        }
                        break;
                    default:
                        break;
                    }
                }
                if (m_fingerPointMapping.isEmpty()) {
                    event->ignore();
                    return false;
                } else {
                    return true;
                }
            }
            break;
        case QEvent::TouchEnd:
            if (m_fingerPointMapping.isEmpty()) {
                event->ignore();
                return false;
            }
            return true;
            break;

        case QEvent::Resize:
        {
            QResizeEvent *e = (QResizeEvent *) event;
            if (e->oldSize().width() == 0 || e->oldSize().height() == 0)
                break;
            qreal stretch_x = e->size().width() / qreal(e->oldSize().width());
            qreal stretch_y = e->size().height() / qreal(e->oldSize().height());
            for (int i=0; i<m_points.size(); ++i) {
                QPointF p = m_points[i];
                movePoint(i, QPointF(p.x() * stretch_x, p.y() * stretch_y), false);
            }

            firePointChange();
            break;
        }
		case QEvent::MouseButtonDblClick:
		{
          
			if (!m_fingerPointMapping.isEmpty())
                return true;
            QMouseEvent *me = (QMouseEvent *) event;

            QPointF clickPos = me->pos();
            int index = -1;
            for (int i=0; i<m_points.size(); ++i) {
                QPainterPath path;
                if (m_shape == CircleShape)
                    path.addEllipse(pointBoundingRect(i));
                else
                    path.addRect(pointBoundingRect(i));

                if (path.contains(clickPos)) {
                    index = i;
				
                    break;
                }
            }
			this ->color_index = index;
			if(this ->is_new_point)
			{
				
				this ->is_new_point = false;
				break;
			}
			this ->color.open();

			

		
			break;
		}

        case QEvent::Paint:
        {
            QWidget *that_widget = m_widget;
            m_widget = 0;
            QApplication::sendEvent(object, event);
            m_widget = that_widget;
            paintPoints();

            return true;
        }
		
        default:
            break;
        }
    }

    return false;
}


void HoverPoints::paintPoints()
{
    QPainter p;

    p.begin(m_widget);

    p.setRenderHint(QPainter::Antialiasing);

    if (m_connectionPen.style() != Qt::NoPen && m_connectionType != NoConnection) {
        p.setPen(m_connectionPen);

        if (m_connectionType == CurveConnection) {
            QPainterPath path;
            path.moveTo(m_points.at(0));
            for (int i=1; i<m_points.size(); ++i) {
                QPointF p1 = m_points.at(i-1);
                QPointF p2 = m_points.at(i);
                qreal distance = p2.x() - p1.x();

                path.cubicTo(p1.x() + distance / 2, p1.y(),
                             p1.x() + distance / 2, p2.y(),
                             p2.x(), p2.y());
            }
            p.drawPath(path);
		
        } else {
            p.drawPolyline(m_points);
        }
    }

    p.setPen(m_pointPen);
    p.setBrush(m_pointBrush);

    for (int i=0; i<m_points.size(); ++i) {
        QRectF bounds = pointBoundingRect(i);
        if (m_shape == CircleShape)
            p.drawEllipse(bounds);
        else
            p.drawRect(bounds);
    }
}

static QPointF bound_point(const QPointF &point, const QRectF &bounds, int lock)
{
    QPointF p = point;

    qreal left = bounds.left();
    qreal right = bounds.right();
    qreal top = bounds.top();
    qreal bottom = bounds.bottom();

    if (p.x() < left || (lock & HoverPoints::LockToLeft)) p.setX(left);
    else if (p.x() > right || (lock & HoverPoints::LockToRight)) p.setX(right);

    if (p.y() < top || (lock & HoverPoints::LockToTop)) p.setY(top);
    else if (p.y() > bottom || (lock & HoverPoints::LockToBottom)) p.setY(bottom);

    return p;
}

void HoverPoints::setPoints(const QPolygonF &points)
{
    if (points.size() != m_points.size())
        m_fingerPointMapping.clear();
    m_points.clear();
    for (int i=0; i<points.size(); ++i)
        m_points << bound_point(points.at(i), boundingRect(), 0);

    m_locks.clear();
    if (m_points.size() > 0) {
        m_locks.resize(m_points.size());

        m_locks.fill(0);
    }
}


void HoverPoints::movePoint(int index, const QPointF &point, bool emitUpdate)
{
    m_points[index] = bound_point(point, boundingRect(), m_locks.at(index));
    if (emitUpdate)
        firePointChange();
}


inline static bool x_less_than(const QPointF &p1, const QPointF &p2)
{
    return p1.x() < p2.x();
}


inline static bool y_less_than(const QPointF &p1, const QPointF &p2)
{
    return p1.y() < p2.y();
}

void HoverPoints::firePointChange()
{

    if (m_sortType != NoSort) {

        QPointF oldCurrent;
        if (m_currentIndex != -1) {
            oldCurrent = m_points[m_currentIndex];
        }

        if (m_sortType == XSort)
            qSort(m_points.begin(), m_points.end(), x_less_than);
        else if (m_sortType == YSort)
            qSort(m_points.begin(), m_points.end(), y_less_than);

        // Compensate for changed order...
        if (m_currentIndex != -1) {
            for (int i=0; i<m_points.size(); ++i) {
                if (m_points[i] == oldCurrent) {
                    m_currentIndex = i;
                    break;
                }
            }
        }


    }


    emit pointsChanged();
}
void HoverPoints::setColor(QColor c)
{
	

	this ->m_colors.replace(color_index,QPair<qreal,QColor>(color_index,c));
   
	 emit colorsChanged();
}