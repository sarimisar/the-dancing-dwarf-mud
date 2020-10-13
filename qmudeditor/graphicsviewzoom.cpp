#include "graphicsviewzoom.h"

#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>

GraphicsViewZoom::GraphicsViewZoom(QGraphicsView* view)
    : QObject(view),
      m_ptrView(view)
{
    m_ptrView->viewport()->installEventFilter(this);
    m_ptrView->setMouseTracking(true);
    m_modifiers = Qt::ControlModifier;
    m_dZoomFactorBase = 1.0015;
}

void GraphicsViewZoom::zoom(double factor)
{
    m_ptrView->scale(factor, factor);
    m_ptrView->centerOn(m_targetScenePos);

    QPointF delta = m_targetViewportPos - QPointF(m_ptrView->viewport()->width() / 2.0, m_ptrView->viewport()->height() / 2.0);
    QPointF center = m_ptrView->mapFromScene(m_targetScenePos) - delta;

    m_ptrView->centerOn(m_ptrView->mapToScene(center.toPoint()));

    emit zoomed();
}

void GraphicsViewZoom::setModifiers(Qt::KeyboardModifiers modifiers)
{
    m_modifiers = modifiers;
}

void GraphicsViewZoom::setZoomFactorBase(double value)
{
    m_dZoomFactorBase = value;
}

bool GraphicsViewZoom::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)

    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        QPointF delta = m_targetViewportPos - mouseEvent->pos();

        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5)
        {
            m_targetViewportPos = mouseEvent->pos();
            m_targetScenePos = m_ptrView->mapToScene(mouseEvent->pos());
        }
    }
    else if (event->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

        if (QApplication::keyboardModifiers() == m_modifiers)
        {
            double angle = wheelEvent->angleDelta().y();

            if (angle != 0)
            {
                double factor = qPow(m_dZoomFactorBase, angle);
                zoom(factor);
                return true;
            }
        }
    }


    return false;
}
