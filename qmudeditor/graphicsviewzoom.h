#ifndef GRAPHICSVIEWZOOM_H
#define GRAPHICSVIEWZOOM_H

#include <QObject>
#include <QGraphicsView>

class GraphicsViewZoom : public QObject
{
    Q_OBJECT

public:
  GraphicsViewZoom(QGraphicsView* view);
  void zoom(double factor);
  void setModifiers(Qt::KeyboardModifiers modifiers);
  void setZoomFactorBase(double value);

signals:
  void zoomed();

protected:
  bool eventFilter(QObject* object, QEvent* event);

private:
  QGraphicsView* m_ptrView;

  Qt::KeyboardModifiers m_modifiers;

  double m_dZoomFactorBase;

  QPointF m_targetScenePos;
  QPointF m_targetViewportPos;
};

#endif // GRAPHICSVIEWZOOM_H
