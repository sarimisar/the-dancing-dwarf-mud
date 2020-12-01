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
#include <QTextLayout>
#include <private/qquicktextnode_p.h>
#include <QFontMetrics>

#include "resourcemanager.h"

MapQuickItem::MapQuickItem(QQuickItem *parent) :
    QQuickItem(parent),
    m_ptrMapImageLevel0Node(Q_NULLPTR),
    m_ptrPcNpcNode(Q_NULLPTR),
    m_ptrMapImageLevel1Node(Q_NULLPTR),
    m_ptrPlayerNode(Q_NULLPTR),
    m_ptrRoomUnderMouseNode(Q_NULLPTR),
    m_ptrRoomSelectedNode(Q_NULLPTR),
    m_bNeedUpdateMiniMap(true),
    m_bNeedUpdateGeometry(true),
    m_bNeedUpdateBackground(true),
    m_bNeedUpdateZoomFactor(true),
    m_bNeedUpdateTailSizePx(true),
    m_bNeedUpdateMapId(true),
    m_bNeedUpdateMapTailSize(true),
    m_bNeedUpdateMapCenterPoint(true),
    m_bNeedUpdateNpcs(true),
    m_bNeedUpdateRoomUnderMouse(true),
    m_bNeedUpdateRoomSelected(true),
    m_bMiniMap(false),
    m_rZoomFactor(1.0),
    m_iTailSizePx(80),
    m_mapTailSize(QSize(50, 50)),
    m_mapCenterPoint(QPoint(24, 24))
{
    setFlag(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);

    m_font.setPointSize(10);
    m_font.setFamily("Cooper Black");
}

void MapQuickItem::declareQML()
{
    qmlRegisterType<MapQuickItem>("qmudmapquickitem.qml", 1, 0, "QMudMap");
}

QString MapQuickItem::npcDataToString(quint64 id, int x, int y, QMUD::ClientDataRoomInfo::ChType type, QString name, QMUD::RaceType race)
{
    return QString("%1,%2,%3,%4,%5,%6")
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

void MapQuickItem::setSelectedRoom(const QPoint &room)
{
    qDebug() << room;

    if (m_bMiniMap)
        return;

    if (room != m_roomSelected)
    {
        m_roomSelected = room;
        m_bNeedUpdateRoomSelected = true;

        update();

        emit selectedRoomChanged(room);
    }
}

QPointF MapQuickItem::mapFromRoomPos(const QPoint &room)
{
    return QPointF(room - m_mapCenterPoint) * m_iTailSizePx * m_rZoomFactor + QPointF(width() / 2.0, height() / 2.0);
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
        m_ptrMapImageLevel0Node->setFiltering(QSGTexture::Linear);
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
        m_ptrMapImageLevel1Node->setFiltering(QSGTexture::Linear);
        m_ptrPcNpcNode->appendChildNode(m_ptrMapImageLevel1Node);

        // Room selection
        QSGOpacityNode *opacityNodeRoomUnderMouse  = new QSGOpacityNode();
        opacityNodeRoomUnderMouse->setOpacity(0.0);
        m_ptrRoomUnderMouseNode = new QSGSimpleTextureNode();
        m_ptrRoomUnderMouseNode->setFiltering(QSGTexture::Linear);
        m_ptrRoomUnderMouseNode->setTexture(window()->createTextureFromImage(ResourceManager::instance().blueRectangle()));
        opacityNodeRoomUnderMouse->appendChildNode(m_ptrRoomUnderMouseNode);
        m_ptrMapImageLevel1Node->appendChildNode(opacityNodeRoomUnderMouse);

        QSGOpacityNode *opacityNodeRoomSelected  = new QSGOpacityNode();
        opacityNodeRoomSelected->setOpacity(0.5);
        m_ptrRoomSelectedNode = new QSGSimpleTextureNode();
        m_ptrRoomSelectedNode->setFiltering(QSGTexture::Linear);
        m_ptrRoomSelectedNode->setTexture(window()->createTextureFromImage(ResourceManager::instance().blueRectangle()));
        opacityNodeRoomSelected->appendChildNode(m_ptrRoomSelectedNode);
        m_ptrMapImageLevel1Node->appendChildNode(opacityNodeRoomSelected);
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
        m_ptrMapImageLevel0Node->setTexture(ResourceManager::instance().map(m_mapId, window()).textureLevel0);
        m_ptrMapImageLevel1Node->setTexture(ResourceManager::instance().map(m_mapId, window()).textureLevel1);

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
        QRectF windowCenterRect(windowCenter.x() - m_iTailSizePx / 2.0 * m_rZoomFactor,
                                windowCenter.y() - m_iTailSizePx / 2.0 * m_rZoomFactor,
                                m_iTailSizePx * m_rZoomFactor,
                                m_iTailSizePx * m_rZoomFactor);

        m_ptrPlayerNode->setRect(windowCenterRect);

        m_bNeedUpdateZoomFactor = false;
        updateNpcsPosition = true;
    }

    if (m_bNeedUpdateNpcs || m_bNeedUpdateMiniMap)
    {
        QFontMetrics fontMetrics(m_font);
        int leading = fontMetrics.leading();

        QVector<QSGSimpleTextureNode*> notRemove;

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
                data.node = new QSGSimpleTextureNode();
                data.node->setFiltering(QSGTexture::Linear);

                if (data.type == QMUD::ClientDataRoomInfo::ChType::NPC)
                    data.node->setTexture(window()->createTextureFromImage(ResourceManager::instance().orangeCircle()));
                else if (data.type == QMUD::ClientDataRoomInfo::ChType::PLAYER)
                    data.node->setTexture(window()->createTextureFromImage(ResourceManager::instance().blueCircle()));
                else
                    data.node->setTexture(window()->createTextureFromImage(ResourceManager::instance().redCircle()));

                m_ptrPcNpcNode->insertChildNodeBefore(data.node, m_ptrPlayerNode);

                if (!m_bMiniMap)
                {
                    QString textLevel = QString("[%1]").arg(10);
                    QString textName = QString("%1").arg(data.name);

                    data.textLayoutLevel = new QTextLayout();
                    data.textLayoutLevel->setText(textLevel);
                    data.textLayoutLevel->setFont(m_font);
                    data.textLayoutLevel->beginLayout();
                    QTextLine lineLevel = data.textLayoutLevel->createLine();
                    lineLevel.setLineWidth(80);
                    lineLevel.setPosition(QPointF((80.0 - fontMetrics.boundingRect(textLevel).width()) / 2.0, leading));
                    data.textLayoutLevel->endLayout();

                    data.textLayoutName = new QTextLayout();
                    data.textLayoutName->setText(textName);
                    data.textLayoutName->setFont(m_font);
                    data.textLayoutName->beginLayout();
                    QTextLine lineName = data.textLayoutName->createLine();
                    lineName.setLineWidth(80);
                    lineName.setPosition(QPointF((80.0 - fontMetrics.boundingRect(textName).width()) / 2.0, leading));
                    data.textLayoutName->endLayout();

                    data.textNode = new QQuickTextNode(this);
                    data.node->appendChildNode(data.textNode);
                }
                else
                {
                    data.textNode = Q_NULLPTR;
                    data.textLayoutName = Q_NULLPTR;
                    data.textLayoutLevel = Q_NULLPTR;
                }

                m_npcsNodes[data.index] = data;

                notRemove.push_back(data.node);
            }
        }

        for (auto it = m_npcsNodes.begin(); it != m_npcsNodes.end(); )
        {
            if (!notRemove.contains(it.value().node))
            {
                m_ptrPcNpcNode->removeChildNode(it.value().node);
                if (it.value().textLayoutName)
                    delete it.value().textLayoutName;
                if (it.value().textLayoutLevel)
                    delete it.value().textLayoutLevel;
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

            if (!m_bMiniMap)
            {
                it.value().textNode->removeAllChildNodes();
                it.value().textNode->addTextLayout(positionRect.topLeft() + QPointF(0.0, 20.0), it.value().textLayoutLevel, Qt::white, QQuickText::Outline);
                it.value().textNode->addTextLayout(positionRect.topLeft() + QPointF(0.0, 35.0), it.value().textLayoutName, Qt::white, QQuickText::Outline);
            }
            else
            {
                if (it.value().textNode)
                    it.value().textNode->removeAllChildNodes();
            }
        }
    }

    if (m_bNeedUpdateRoomSelected || m_bNeedUpdateMapCenterPoint)
    {
        if (!m_bMiniMap)
        {
            QSizeF imageSize = QSizeF(m_mapTailSize) * m_iTailSizePx * m_rZoomFactor;

            qreal x = - (imageSize.width() - width()) / 2.0;
            qreal y = - (imageSize.height() - height()) / 2.0;

            QPointF centerPoint = (QPointF(m_mapCenterPoint * m_iTailSizePx)  +
                    QPointF(m_iTailSizePx / 2.0, m_iTailSizePx / 2.0)) * m_rZoomFactor;

            // Scift della mappa rispetto al centro voluto
            x -= (centerPoint.x() - (imageSize.width() / 2.0));
            y -= (centerPoint.y() - (imageSize.height() / 2.0));

            QPointF position = (QPointF(m_roomSelected * m_iTailSizePx)) * m_rZoomFactor;

            QRectF positionRect(x + position.x(),
                                y + position.y(),
                                m_iTailSizePx * m_rZoomFactor,
                                m_iTailSizePx * m_rZoomFactor);

            m_ptrRoomSelectedNode->setRect(positionRect);
        }
        else
        {
            m_ptrRoomSelectedNode->setRect(0, 0, 0, 0);
        }

        m_bNeedUpdateRoomSelected = false;
    }

    if (m_bNeedUpdateRoomUnderMouse || m_bNeedUpdateMapCenterPoint)
    {
        if (!m_bMiniMap)
        {
            QSizeF imageSize = QSizeF(m_mapTailSize) * m_iTailSizePx * m_rZoomFactor;

            qreal x = - (imageSize.width() - width()) / 2.0;
            qreal y = - (imageSize.height() - height()) / 2.0;

            QPointF centerPoint = (QPointF(m_mapCenterPoint * m_iTailSizePx)  +
                    QPointF(m_iTailSizePx / 2.0, m_iTailSizePx / 2.0)) * m_rZoomFactor;

            // Scift della mappa rispetto al centro voluto
            x -= (centerPoint.x() - (imageSize.width() / 2.0));
            y -= (centerPoint.y() - (imageSize.height() / 2.0));

            QPointF position = (QPointF(m_roomUnderMouse * m_iTailSizePx)) * m_rZoomFactor;

            QRectF positionRect(x + position.x(),
                                y + position.y(),
                                m_iTailSizePx * m_rZoomFactor,
                                m_iTailSizePx * m_rZoomFactor);

            m_ptrRoomUnderMouseNode->setRect(positionRect);
        }
        else
        {
            m_ptrRoomUnderMouseNode->setRect(0, 0, 0, 0);
        }

        m_bNeedUpdateRoomUnderMouse = false;
    }

    m_bNeedUpdateMiniMap = false;
    m_bNeedUpdateMapCenterPoint = false;


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

void MapQuickItem::mousePressEvent(QMouseEvent *event)
{

}

void MapQuickItem::hoverMoveEvent(QHoverEvent* event)
{
    if (m_bMiniMap)
        return;

    auto pos = QPointF(event->pos()) / m_rZoomFactor;

    pos -= QPointF(width() / 2.0, height() / 2.0);
    pos /= m_iTailSizePx;

    auto newRoom = m_mapCenterPoint + pos.toPoint();

    if (newRoom != m_roomUnderMouse)
    {
        m_roomUnderMouse = newRoom;
        m_bNeedUpdateRoomUnderMouse = true;

        update();
    }
}

void MapQuickItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_bMiniMap)
        return;

    auto pos = QPointF(event->pos()) / m_rZoomFactor;

    pos -= QPointF(width() / 2.0, height() / 2.0);
    pos /= m_iTailSizePx;

    auto newRoom = m_mapCenterPoint + pos.toPoint();

    if (newRoom != m_roomSelected)
    {
        m_roomSelected = newRoom;
        m_bNeedUpdateRoomSelected = true;

        update();

        emit selectedRoomChanged(newRoom);
    }
}

bool MapQuickItem::miniMap() const
{
    return m_bMiniMap;
}

void MapQuickItem::setMiniMap(bool miniMap)
{
    if (m_bMiniMap != miniMap)
    {
        m_bMiniMap = miniMap;
        m_bNeedUpdateMiniMap = true;

        update();

        emit miniMapChanged();
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
    if (!qFuzzyCompare(m_rZoomFactor, factor))
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
    if (m_mapId != id)
    {
        m_mapId = id;
        m_bNeedUpdateMapId = true;

        update();

        emit mapIdChanged();
    }
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

        emit npcsChanged();
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
