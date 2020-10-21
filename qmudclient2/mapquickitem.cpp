#include "mapquickitem.h"

#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>

MapQuickItem::MapQuickItem(QQuickItem *parent) :
    QQuickItem(parent),
    m_bNeedUpdateGeometry(true),
    m_bNeedUpdateBackground(true),
    m_bNeedUpdateZoomFactor(true),
    m_bNeedUpdateTailSizePx(true),
    m_bNeedUpdateMapId(true),
    m_bNeedUpdateMapTailSize(true),
    m_bNeedUpdateMapCenterPoint(true),
    m_rZoomFactor(1.0),
    m_iTailSizePx(80),
    m_mapTailSize(QSize(50, 50)),
    m_mapCenterPoint(QPoint(24, 24))
{
    setFlag(QQuickItem::ItemHasContents);
}

void MapQuickItem::declareQML()
{
    qmlRegisterType<MapQuickItem>("qmudmapquickitem.qml", 1, 0, "QMudMap");
}

QSGNode *MapQuickItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    QSGSimpleRectNode *root = static_cast<QSGSimpleRectNode*>(oldNode);

    if (!root)
    {
        // Background
        root = new QSGSimpleRectNode();

        // CHILD [0]
        // Map Image level 0
        QSGSimpleTextureNode *mapImageLevel0Node = new QSGSimpleTextureNode();
        mapImageLevel0Node->setOwnsTexture(true);
        mapImageLevel0Node->setTexture(window()->createTextureFromImage(m_mapImageLevel0));
        root->appendChildNode(mapImageLevel0Node);

        // CHILD [1]
        // Map Image level 1
        QSGSimpleTextureNode *mapImageLevel1Node = new QSGSimpleTextureNode();
        mapImageLevel1Node->setOwnsTexture(true);
        mapImageLevel1Node->setTexture(window()->createTextureFromImage(m_mapImageLevel1));
        mapImageLevel0Node->appendChildNode(mapImageLevel1Node);
    }

    QSGSimpleTextureNode *mapImageLevel0Node = static_cast<QSGSimpleTextureNode*>(root->childAtIndex(0));
    QSGSimpleTextureNode *mapImageLevel1Node = static_cast<QSGSimpleTextureNode*>(mapImageLevel0Node->childAtIndex(0));

    if (m_bNeedUpdateGeometry)
    {
        root->setRect(0, 0, width(), height());

        m_bNeedUpdateGeometry = false;
        m_bNeedUpdateMapCenterPoint = true;
    }

    if (m_bNeedUpdateBackground)
    {
        root->setColor(m_backgroundColor);

        m_bNeedUpdateBackground = false;
    }

    if (m_bNeedUpdateMapId)
    {
        mapImageLevel0Node->setTexture(window()->createTextureFromImage(m_mapImageLevel0));
        mapImageLevel1Node->setTexture(window()->createTextureFromImage(m_mapImageLevel1));

        m_bNeedUpdateMapId = false;

        // Forzo l'aggiornamento del center point dato che dipende dalla m_mapImage.size()
        m_bNeedUpdateMapCenterPoint = true;
    }

    if (m_bNeedUpdateZoomFactor || m_bNeedUpdateMapCenterPoint)
    {
        QSizeF imageSize = QSizeF(m_mapTailSize) * m_iTailSizePx * m_rZoomFactor;

        QPointF centerPoint = (QPointF(m_mapCenterPoint * m_iTailSizePx)  +
                QPointF(m_iTailSizePx / 2.0, m_iTailSizePx / 2.0)) * m_rZoomFactor;

        qreal x = - (imageSize.width() - width()) / 2.0;
        qreal y = - (imageSize.height() - height()) / 2.0;

        // Scift della mappa rispetto al centro voluto
        x -= (centerPoint.x() - (imageSize.width() / 2.0));
        y -= (centerPoint.y() - (imageSize.height() / 2.0));

        mapImageLevel0Node->setRect(x, y, imageSize.width(), imageSize.height());
        mapImageLevel1Node->setRect(x, y, imageSize.width(), imageSize.height());

        m_bNeedUpdateZoomFactor = false;
        m_bNeedUpdateMapCenterPoint = false;
    }

    return root;
}

void MapQuickItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (newGeometry != oldGeometry)
    {
        m_bNeedUpdateGeometry = true;
        update();
    }
}

QColor MapQuickItem::background() const
{
    return m_backgroundColor;
}

void MapQuickItem::setBackground(const QColor &color)
{
    if (m_backgroundColor != color)
    {
        m_backgroundColor = color;
        m_bNeedUpdateBackground = true;

        update();

        emit backgroundChanged();
    }
}

qreal MapQuickItem::zoomFactor() const
{
    return m_rZoomFactor;
}

void MapQuickItem::setZoomFactor(qreal factor)
{
    if (qFuzzyCompare(m_rZoomFactor, factor))
    {
        m_rZoomFactor = factor;
        m_bNeedUpdateZoomFactor = true;

        update();

        emit zoomFactorChanged();
    }
}

int MapQuickItem::tailSizePx() const
{
    return m_iTailSizePx;
}

void MapQuickItem::setTailSizePx(int tailSize)
{
    if (m_iTailSizePx != tailSize)
    {
        m_iTailSizePx = tailSize;
        m_bNeedUpdateTailSizePx = true;

        update();

        emit tailSizePxChanged();
    }
}

QPoint MapQuickItem::mapId() const
{
    return m_mapId;
}

void MapQuickItem::setMapId(QPoint id)
{
    m_mapId = id;

    bool found = false;

    for (auto mapData : m_vLoadedMaps)
        if (mapData.id == id)
        {
            m_mapImageLevel0 = mapData.imageLevel0;
            m_mapImageLevel1 = mapData.imageLevel1;
            found = true;
            break;
        }

    if (!found)
    {
        LoadedMapData mapData;
        mapData.id = id;

        QString fileMapLevel0 = QString("./maps/%1_%2_%3.png").arg(id.x()).arg(id.y()).arg(0);
        QString fileMapLevel1 = QString("./maps/%1_%2_%3.png").arg(id.x()).arg(id.y()).arg(1);

        mapData.imageLevel0 = QImage(fileMapLevel0);
        mapData.imageLevel1 = QImage(fileMapLevel1);

        m_mapImageLevel0 = mapData.imageLevel0;
        m_mapImageLevel1 = mapData.imageLevel1;

        m_vLoadedMaps.push_back(mapData);

        if (m_vLoadedMaps.size() > 5)
            m_vLoadedMaps.pop_front();
    }

    m_bNeedUpdateMapId = true;

    update();

    emit mapIdChanged();
}

QSize MapQuickItem::mapTailSize() const
{
    return m_mapTailSize;
}

void MapQuickItem::setMapTailSize(const QSize &size)
{
    if (m_mapTailSize != size)
    {
        m_mapTailSize = size;
        m_bNeedUpdateMapTailSize = true;

        update();

        emit mapTailSizeChanged();
    }
}

QPoint MapQuickItem::mapCenterPoint() const
{
    return m_mapCenterPoint;
}

void MapQuickItem::setMapCenterPoint(const QPoint &point)
{
    if (m_mapCenterPoint != point)
    {
        m_mapCenterPoint = point;
        m_bNeedUpdateMapCenterPoint = true;

        update();

        emit mapCenterPointChanged();
    }
}
