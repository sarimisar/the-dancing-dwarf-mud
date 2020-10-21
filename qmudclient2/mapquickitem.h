#ifndef MAPQUICKITEM_H
#define MAPQUICKITEM_H

#include <QQuickItem>
#include <QPixmap>

class MapQuickItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)

    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(int tailSizePx READ tailSizePx WRITE setTailSizePx NOTIFY tailSizePxChanged)

    Q_PROPERTY(QPoint mapId READ mapId WRITE setMapId NOTIFY mapIdChanged)
    Q_PROPERTY(QSize mapTailSize READ mapTailSize WRITE setMapTailSize NOTIFY mapTailSizeChanged)
    Q_PROPERTY(QPoint mapCenterPoint READ mapCenterPoint WRITE setMapCenterPoint NOTIFY mapCenterPointChanged)

public:
    MapQuickItem(QQuickItem *parent = Q_NULLPTR);

    static void declareQML();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    QColor background() const;
    void setBackground(const QColor &color);

    qreal zoomFactor() const;
    void setZoomFactor(qreal factor);

    int tailSizePx() const;
    void setTailSizePx(int tailSizePx);

    QPoint mapId() const;
    void setMapId(QPoint id);

    QSize mapTailSize() const;
    void setMapTailSize(const QSize& size);

    QPoint mapCenterPoint() const;
    void setMapCenterPoint(const QPoint &point);

signals:
    void backgroundChanged();
    void zoomFactorChanged();
    void tailSizePxChanged();
    void mapIdChanged();
    void mapTailSizeChanged();
    void mapCenterPointChanged();

private:
    bool m_bNeedUpdateGeometry;
    bool m_bNeedUpdateBackground;
    bool m_bNeedUpdateZoomFactor;
    bool m_bNeedUpdateTailSizePx;
    bool m_bNeedUpdateMapId;
    bool m_bNeedUpdateMapTailSize;
    bool m_bNeedUpdateMapCenterPoint;

    QColor m_backgroundColor;
    qreal m_rZoomFactor;
    int m_iTailSizePx;
    QSize m_mapTailSize;
    QPoint m_mapId;
    QImage m_mapImageLevel0;
    QImage m_mapImageLevel1;
    QPoint m_mapCenterPoint;

    struct LoadedMapData
    {
        QImage imageLevel0;
        QImage imageLevel1;
        QPoint id;
    };

    QVector<LoadedMapData> m_vLoadedMaps;
};

#endif // MAPQUICKITEM_H
