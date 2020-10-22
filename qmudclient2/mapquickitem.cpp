#include "mapquickitem.h"

#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QFlags>
#include <QThread>

#include "resourcemanager.h"

MapQuickItem::MapQuickItem(QQuickItem *parent) :
    QQuickItem(parent),
    m_ptrMapImageLevel0Node(Q_NULLPTR),
    m_ptrPcNpcNode(Q_NULLPTR),
    m_ptrMapImageLevel1Node(Q_NULLPTR),
    m_ptrPlayerNode(Q_NULLPTR),
    m_bNeedUpdateGeometry(true),
    m_bNeedUpdateBackground(true),
    m_bNeedUpdateZoomFactor(true),
    m_bNeedUpdateTailSizePx(true),
    m_bNeedUpdateMapId(true),
    m_bNeedUpdateMapTailSize(true),
    m_bNeedUpdateMapCenterPoint(true),
    m_bNeedUpdateNpcs(true),
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

QString MapQuickItem::npcDataToString(quint64 id, int x, int y, QMUD::ClientDataRoomInfo::ChType type, QString name, QMUD::RaceType race)
{
    return QString("%1,%2,%3,%4,%4,%6")
            .arg(id)
            .arg(x)
            .arg(y)
            .arg(static_cast<int>(type))
            .arg(name)
            .arg(static_cast<int>(race));
}

void MapQuickItem::npcDataFromString(QString str, quint64 &id, int &x, int &y, QMUD::ClientDataRoomInfo::ChType &type, QString &name, QMUD::RaceType &race)
{
    QStringList split = str.split(",");

    id = split[0].toULongLong();
    x = split[1].toInt();
    y = split[2].toInt();
    type = static_cast<QMUD::ClientDataRoomInfo::ChType>(split[3].toInt());
    name = split[4];
    race = static_cast<QMUD::RaceType>(split[5].toInt());
}

QSGNode *MapQuickItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    QSGSimpleRectNode *root = static_cast<QSGSimpleRectNode*>(oldNode);

    bool updateNpcsPosition = false;

    if (!root)
    {
        // Background
        root = new QSGSimpleRectNode();

        // Map image level 0
        m_ptrMapImageLevel0Node = new QSGSimpleTextureNode();
        m_ptrMapImageLevel0Node->setOwnsTexture(true);
        m_ptrMapImageLevel0Node->setFiltering(QSGTexture::Linear);
        m_ptrMapImageLevel0Node->setTexture(window()->createTextureFromImage(m_mapImageLevel0));
        root->appendChildNode(m_ptrMapImageLevel0Node);

        // PC/NPC
        m_ptrPcNpcNode = new QSGOpacityNode();
        m_ptrPcNpcNode->setOpacity(0.8);
        m_ptrMapImageLevel0Node->appendChildNode(m_ptrPcNpcNode);

        {
            // Player
            m_ptrPlayerNode = new QSGSimpleTextureNode();
            m_ptrPlayerNode->setFiltering(QSGTexture::Linear);
            m_ptrPlayerNode->setTexture(window()->createTextureFromImage(ResourceManager::instance().whiteCircle()));
            m_ptrPcNpcNode->appendChildNode(m_ptrPlayerNode);
        }

        // Map Image level 1
        m_ptrMapImageLevel1Node = new QSGSimpleTextureNode();
        m_ptrMapImageLevel1Node->setOwnsTexture(true);
        m_ptrMapImageLevel1Node->setFiltering(QSGTexture::Linear);
        m_ptrMapImageLevel1Node->setTexture(window()->createTextureFromImage(m_mapImageLevel1));
        m_ptrPcNpcNode->appendChildNode(m_ptrMapImageLevel1Node);
    }

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
        m_ptrMapImageLevel0Node->setTexture(window()->createTextureFromImage(m_mapImageLevel0));
        m_ptrMapImageLevel1Node->setTexture(window()->createTextureFromImage(m_mapImageLevel1));

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

        m_ptrMapImageLevel0Node->setRect(x, y, imageSize.width(), imageSize.height());
        m_ptrMapImageLevel1Node->setRect(x, y, imageSize.width(), imageSize.height());

        QPointF windowCenter(width() / 2.0, height() / 2.0);
        QRectF windowCenterRect((windowCenter.x() - m_iTailSizePx / 2.0) * m_rZoomFactor,
                                (windowCenter.y() - m_iTailSizePx / 2.0) * m_rZoomFactor,
                                m_iTailSizePx * m_rZoomFactor,
                                m_iTailSizePx * m_rZoomFactor);

        m_ptrPlayerNode->setRect(windowCenterRect);

        m_bNeedUpdateZoomFactor = false;
        m_bNeedUpdateMapCenterPoint = false;
        updateNpcsPosition = true;
    }

    if (m_bNeedUpdateNpcs)
    {
        QVector<QSGSimpleTextureNode*> notRemove;

        qDebug() << m_vNpcs;

        for (auto strNpc : m_vNpcs)
        {
            NpcNodeData data;
            data.fromString(strNpc);
            auto it = m_npcsNodes.find(data.index);

            if (it != m_npcsNodes.end())
            {
                it.value().position = data.position;

                notRemove.push_back(it.value().node);
            }
            else
            {
                QSGSimpleTextureNode *node = new QSGSimpleTextureNode();
                node->setFiltering(QSGTexture::Linear);

                if (data.type == QMUD::ClientDataRoomInfo::ChType::NPC)
                    node->setTexture(window()->createTextureFromImage(ResourceManager::instance().orangeCircle()));
                else if (data.type == QMUD::ClientDataRoomInfo::ChType::PLAYER)
                    node->setTexture(window()->createTextureFromImage(ResourceManager::instance().blueCircle()));
                else
                    node->setTexture(window()->createTextureFromImage(ResourceManager::instance().redCircle()));

                m_ptrPcNpcNode->insertChildNodeBefore(node, m_ptrPlayerNode);

                data.node = node;
                m_npcsNodes[data.index] = data;

                notRemove.push_back(node);
            }
        }

        for (auto it = m_npcsNodes.begin(); it != m_npcsNodes.end(); )
        {
            if (!notRemove.contains(it.value().node))
            {
                m_ptrPcNpcNode->removeChildNode(it.value().node);
                it = m_npcsNodes.erase(it);
            }
            else if (it != m_npcsNodes.end())
                ++it;
        }

        updateNpcsPosition = true;
        m_bNeedUpdateNpcs = false;
    }

    if (updateNpcsPosition)
    {
        QSizeF imageSize = QSizeF(m_mapTailSize) * m_iTailSizePx * m_rZoomFactor;

        qreal x = - (imageSize.width() - width()) / 2.0;
        qreal y = - (imageSize.height() - height()) / 2.0;

        QPointF centerPoint = (QPointF(m_mapCenterPoint * m_iTailSizePx)  +
                QPointF(m_iTailSizePx / 2.0, m_iTailSizePx / 2.0)) * m_rZoomFactor;

        // Scift della mappa rispetto al centro voluto
        x -= (centerPoint.x() - (imageSize.width() / 2.0));
        y -= (centerPoint.y() - (imageSize.height() / 2.0));

        for (auto it = m_npcsNodes.begin(); it != m_npcsNodes.end(); ++it)
        {
            QPointF position = (QPointF(it.value().position * m_iTailSizePx)) * m_rZoomFactor;

            QRectF positionRect(x + position.x(),
                                y + position.y(),
                                m_iTailSizePx * m_rZoomFactor,
                                m_iTailSizePx * m_rZoomFactor);

            it.value().node->setRect(positionRect);
        }
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

QStringList MapQuickItem::npcs() const
{
    return m_vNpcs;
}

void MapQuickItem::setNpcs(QStringList npcs)
{
    if (m_vNpcs != npcs)
    {
        m_vNpcs = npcs;
        m_bNeedUpdateNpcs = true;

        update();

        emit npcsChange();
    }
}

QString MapQuickItem::NpcNodeData::toString() const
{
    return npcDataToString(index, position.x(), position.y(), type, name, race);
}

void MapQuickItem::NpcNodeData::fromString(QString str)
{
    int x, y;
    npcDataFromString(str, index, x, y, type, name, race);
    position = QPoint(x, y);
}
