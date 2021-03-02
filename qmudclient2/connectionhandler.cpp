#include "connectionhandler.h"
#include "../common.h"
#include "mapquickitem.h"

void ConnectionHandler::declareQML()
{
    qmlRegisterType<ConnectionHandler>("QMudEnums", 1, 0, "PageType");
}

void ConnectionHandler::declareQMLContext(QQmlEngine *engine)
{
    engine->rootContext()->setContextProperty("charactersModel", QVariant::fromValue(&m_charactersModel));
}

ConnectionHandler::ConnectionHandler(QObject *parent) :
    QObject(parent),
    m_ptrConnection(new Connection(this)),
    m_bIsConnected(false),
    m_eCurrentPage(PageType::LOGIN),
    m_iPlayerLevel(0),
    m_iPlayerExperience(0),
    m_iPlayerExperienceToLevelUp(0),
    m_iPlayerHpCurrent(0),
    m_iPlayerHpMaximum(0),
    m_iPlayerMpCurrent(0),
    m_iPlayerMpMaximum(0),
    m_iPlayerApCurrent(0),
    m_iPlayerApMaximum(0),
    m_iSelectedPcId(QMUD::IdInvalid),
    m_iSelectedPcLevel(0),
    m_iSelectedPcHpCurrent(0),
    m_iSelectedPcHpMaximum(0),
    m_iSelectedPcMpCurrent(0),
    m_iSelectedPcMpMaximum(0),
    m_iSelectedPcApCurrent(0),
    m_iSelectedPcApMaximum(0)
{
    connect(m_ptrConnection, &Connection::connected, this, &ConnectionHandler::onConnected);
    connect(m_ptrConnection, &Connection::disconnected, this, &ConnectionHandler::onDisconnected);
    connect(m_ptrConnection, &Connection::messageReceived, this, &ConnectionHandler::onMessageReceived);
}

void ConnectionHandler::clearMessages()
{
    m_strLastErrorMessage.clear();
    m_strLastInfoMessage.clear();

    emit lastMessageChange();
}

void ConnectionHandler::cmdLogin(QString username, QString password)
{
    m_ptrConnection->send(QString("login %1 %2").arg(username).arg(password));
}

void ConnectionHandler::cmdSignin(QString username, QString password)
{
    m_ptrConnection->send(QString("signin %1 %2").arg(username).arg(password));
}

void ConnectionHandler::cmdChCreate(QString name, int race, int chClass, int sex)
{
    m_ptrConnection->send(QString("ch_create %1 %2 %3 %4").arg(name)
                          .arg(QMUD::classTypeToString(static_cast<QMUD::ClassType>(chClass)))
                          .arg(QMUD::Race::toString(static_cast<QMUD::RaceType>(race)))
                          .arg(QMUD::sexTypeToString(static_cast<QMUD::SexType>(sex))));
}

void ConnectionHandler::cmdConnect(QString chName)
{
    m_ptrConnection->send(QString("ch_connect %1").arg(chName));
}

void ConnectionHandler::cmdMoveNorth()
{
    m_ptrConnection->send(QString("moven"));
}

void ConnectionHandler::cmdMoveSouth()
{
    m_ptrConnection->send(QString("moves"));
}

void ConnectionHandler::cmdMoveWeast()
{
    m_ptrConnection->send(QString("movew"));
}

void ConnectionHandler::cmdMoveEast()
{
    m_ptrConnection->send(QString("movee"));
}

void ConnectionHandler::cmdMoveUp()
{
    m_ptrConnection->send(QString("moveu"));
}

void ConnectionHandler::cmdMoveDown()
{
    m_ptrConnection->send(QString("moved"));
}

void ConnectionHandler::cmdTarget(QMUD::StaticIdType id)
{
    m_ptrConnection->send(QString("target %1").arg(id));
}

void ConnectionHandler::cmdAction(QString action)
{
    m_ptrConnection->send(QString(action));
}

bool ConnectionHandler::connectToHost()
{
    if (!m_ptrConnection->isConnected())
        return m_ptrConnection->connectToHost("localhost", 8060) ||
            m_ptrConnection->connectToHost("2.224.131.179", 8060);

    return true;
}

void ConnectionHandler::onConnected()
{
    m_bIsConnected = true;

    emit connectionStatusChange();
}

void ConnectionHandler::onDisconnected()
{
    m_bIsConnected = false;

    emit connectionStatusChange();
}

void ConnectionHandler::onMessageReceived(QByteArray message)
{
    QMUD::ClientData* pkt = QMUD::ClientData::deserialize(message);

    if (pkt)
    {
        if (pkt->type() == QMUD::ClientDataType::CDT_MESSAGE)
        {
            QMUD::ClientDataMessage *data = static_cast<QMUD::ClientDataMessage*>(pkt);

            switch (data->message())
            {
            case QMUD::ClientDataMessage::Message::ERROR_GENERIC:
                m_strLastErrorMessage = tr("Si è verificato un errore");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED:
                m_strLastErrorMessage = tr("Nome utente o password errati");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_SIGNIN_FAILED:
                m_strLastErrorMessage = tr("Errore durante la registrazione");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED_CONTACT_ADMINISTRATOR:
                m_strLastErrorMessage = tr("Connessione fallita, contatta un amministratore");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_USERNAME_FAILED:
                m_strLastErrorMessage = tr("Username non valido");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_USERNAME_EXISTS:
                m_strLastErrorMessage = tr("Username già esistente");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_PASSWORD_FAILED:
                m_strLastErrorMessage = tr("Password non valida");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED:
                m_strLastErrorMessage = tr("Connessione fallita, nome utente non valido");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED_CONTACT_ADMINISTRATOR:
                m_strLastErrorMessage = tr("Connessione fallita, contatta un amministratore");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR:
                m_strLastErrorMessage = tr("Creazione fallita, contatta un amministratore");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::ERROR_CH_NAME_FAILED:
                m_strLastErrorMessage = tr("Nome del personaggio non disponibile");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::INFO_SIGNIN_OK:
                m_strLastInfoMessage = tr("Registrazione avvenuta con successo");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::INFO_CH_CREATION_OK:
                m_strLastInfoMessage = tr("Creazione del personaggio riuscita");
                emit lastMessageChange();
                break;

            case QMUD::ClientDataMessage::Message::CONN_STATUS_IDLE:
            case QMUD::ClientDataMessage::Message::CONN_STATUS_LOGIN:
                m_eCurrentPage = PageType::LOGIN;
                emit currentPageChange();
                break;

            case QMUD::ClientDataMessage::Message::CONN_STATUS_CHARACTERS_LIST:
                m_eCurrentPage = PageType::CHARACTERS_LIST;
                emit currentPageChange();
                break;

            case QMUD::ClientDataMessage::Message::CONN_STATUS_IN_GAME:
                m_eCurrentPage = PageType::IN_GAME;
                emit currentPageChange();
                break;
            }
        }
        else if (pkt->type() == QMUD::ClientDataType::CHARACTERS_LIST)
        {
            QMUD::ClientDataCharactersList *data = static_cast<QMUD::ClientDataCharactersList*>(pkt);
            auto list = data->chsList();

            m_charactersModel.clear();

            for (auto elem : list)
                m_charactersModel.addCharacter(CharacterInfo(elem.name(),
                                                             elem.level(),
                                                             QMUD::classTypeToReadableString(elem.classType()),
                                                             QMUD::Race::toReadableString(elem.race())));
        }
        else if (pkt->type() == QMUD::ClientDataType::ROOM_INFO)
        {
            QMUD::ClientDataRoomInfo *data = static_cast<QMUD::ClientDataRoomInfo*>(pkt);

            QPoint newPos = QPoint(data->x(), data->y());

            if (newPos != m_ptPlayerPosition)
            {
                m_ptPlayerPosition = newPos;
                emit playerPositionChange();
            }

            QPoint newMapId = QPoint(static_cast<int>(data->map()), static_cast<int>(data->index()));

            if (newMapId != m_ptPlayerMapId)
            {
                m_ptPlayerMapId = newMapId;
                emit playerMapIdChange();
            }

            auto chs = data->chs();

            QStringList chsList;

            for (auto ch : chs)
            {
                chsList.push_back(MapQuickItem::npcDataToString(ch.id, ch.x, ch.y, ch.type, ch.name, ch.race));
            }

            if (chsList != m_vNpcs)
            {
                m_vNpcs = chsList;
                emit npcsChange();
            }
        }
        else if (pkt->type() == QMUD::ClientDataType::LIFE)
        {
            QMUD::ClientDataLife *data = static_cast<QMUD::ClientDataLife*>(pkt);

            m_iPlayerHpCurrent = data->hpCurrent();
            m_iPlayerHpMaximum = data->hpCurrentMaximum();
            m_iPlayerMpCurrent = data->mpCurrent();
            m_iPlayerMpMaximum = data->mpCurrentMaximum();
            m_iPlayerApCurrent = data->apCurrent();
            m_iPlayerApMaximum = data->apCurrentMaximum();

            emit playerLifeChange();
        }
        else if (pkt->type() == QMUD::ClientDataType::BASIC_INFO)
        {
            QMUD::ClientDataBasicInfo *data = static_cast<QMUD::ClientDataBasicInfo*>(pkt);

            m_strPlayerName = data->name();
            m_iPlayerLevel = data->level();
            m_strPlayerClass = QMUD::classTypeToReadableString(data->classType());
            m_strPlayerRace = QMUD::Race::toReadableString(data->race());
            m_iPlayerExperience = data->experience();
            m_iPlayerExperienceToLevelUp = data->experienceToLevelUp();

            emit playerBasicInfoChange();
        }
        else if (pkt->type() == QMUD::ClientDataType::TARGET_INFO)
        {
            QMUD::ClientDataTargetInfo *data = static_cast<QMUD::ClientDataTargetInfo*>(pkt);

            if (data->name().isEmpty())
            {
                m_iSelectedPcId = QMUD::IdInvalid;
                m_strSelectedPcName.clear();
            }
            else
            {
                m_iSelectedPcId = data->id();
                m_strSelectedPcName = data->name();
                m_iSelectedPcLevel = data->level();
                m_strSelectedPcClass = "";
                m_strSelectedPcRace = "";
                m_iSelectedPcHpCurrent = data->hpCurrent();
                m_iSelectedPcHpMaximum = data->hpCurrentMaximum();
                m_iSelectedPcMpCurrent = data->mpCurrent();
                m_iSelectedPcMpMaximum = data->mpCurrentMaximum();
                m_iSelectedPcApCurrent = data->apCurrent();
                m_iSelectedPcApMaximum = data->apCurrentMaximum();
            }

            emit selectedPcChange();
        }
//        else if (pkt->type() == QMUD::ClientDataType::ACTION_LAG)
//        {
//            QMUD::ClientDataActionLag *data = static_cast<QMUD::ClientDataActionLag*>(pkt);

//            if (!data->update())
//            {
//                ui->progressBarLag->setMaximum(data->actionLag());
//                ui->progressBarLag->setValue(data->actionLag());
//            }
//            else
//                ui->progressBarLag->setValue(data->actionLag());

//            if (ui->progressBarLag->value() > 0)
//                m_timerUpdateLag.start();
//        }
//        else if (pkt->type() == QMUD::ClientDataType::LOOT)
//        {
//            QMUD::ClientDataLoot *data = static_cast<QMUD::ClientDataLoot*>(pkt);

//            ui->textBrowserLoot->clear();
//            ui->textBrowserLootTrash->clear();

//            auto loot = data->lootList();
//            int index = 0;

//            for (auto item : loot)
//            {
//                if (!item.toSell)
//                {
//                    ui->textBrowserLoot->append(QString("<a href=\"bottino tieni %1\"><font color=\"green\">tieni</font></a> ").arg(index) +
//                                             QString("<a href=\"bottino scarta %1\"><font color=\"red\">scarta</font></a> ").arg(index)  +
//                                             QString("<font color=\"%1\">").arg(QMUD::itemQualityToHTMLColor(item.quality)) +
//                                                item.name + "</font>");
//                }
//                else
//                {
//                    ui->textBrowserLootTrash->append(QString("<a href=\"bottino recupera %1\"><font color=\"green\">recupera</font></a> ").arg(index) +
//                                             QString("<font color=\"%1\">").arg(QMUD::itemQualityToHTMLColor(item.quality)) + item.name + "</font>");
//                }

//                index++;
//            }
//        }
//        else if (pkt->type() == QMUD::ClientDataType::STATUS)
//        {
//            QMUD::ClientDataStatus *data = static_cast<QMUD::ClientDataStatus*>(pkt);

//            m_mapStatus.clear();

//            auto statusList = data->statusList();

//            for (auto elem : statusList)
//                m_mapStatus[elem.status] = elem.time * 5000;

//            m_timerUpdateStatus.start();
//        }
//        else if (pkt->type() == QMUD::ClientDataType::AGGRO)
//        {
//            QMUD::ClientDataTargetAggro *data = static_cast<QMUD::ClientDataTargetAggro*>(pkt);

//            if (data->aggroList().isEmpty())
//            {
//                ui->textBrowserAggro->clear();
//            }
//            else
//            {
//                ui->textBrowserAggro->clear();

//                auto aggroList = data->aggroList();

//                std::sort(aggroList.begin(), aggroList.end(), [](const QMUD::ClientDataTargetAggro::AggroData& a, const QMUD::ClientDataTargetAggro::AggroData& b)
//                { return a.aggro > b.aggro; });

//                qint64 maxAggro = 1;
//                for (auto elem : aggroList)
//                    if (elem.aggro > maxAggro)
//                        maxAggro = elem.aggro;

//                for (auto elem : aggroList)
//                {
//                    QString bar;
//                    int count = static_cast<int>((double(elem.aggro) / maxAggro) * 12) + 1;
//                    int percent = static_cast<int>((double(elem.aggro) / maxAggro) * 100);
//                    ui->textBrowserAggro->append(QString("%1: %2% %3").arg(elem.name, 10).arg(percent, 3).arg("", count, '='));
//                }
//            }
//        }
//        else if (pkt->type() == QMUD::ClientDataType::ROOM_INFO)
//        {
//            QMUD::ClientDataRoomInfo *data = static_cast<QMUD::ClientDataRoomInfo*>(pkt);

//            appendEscapeSequenceToTextEdit(ui->textBrowserMain, QString("[LOAD MAP %1.%2 (%3, %4)]").arg(data->map()).arg(data->index()).arg(data->x()).arg(data->y()));

//            if (data->map() != m_iMapIdOnScene || data->index() != m_iMapIndexOnScene)
//            {
//                auto itLevel0Maps = m_mapMapsLevel0.find(data->map());
//                auto itLevel1Maps = m_mapMapsLevel1.find(data->map());

//                if (itLevel0Maps != m_mapMapsLevel0.end() &&
//                    itLevel1Maps != m_mapMapsLevel1.end())
//                {
//                    m_iMapIdOnScene = data->map();
//                    m_iMapIndexOnScene = data->map();

//                    auto itLevel0 = itLevel0Maps->find(data->index());
//                    auto itLevel1 = itLevel1Maps->find(data->index());

//                    m_graphicScene.setSceneRect(itLevel0->rect());

//                    if (!m_ptrGraphicsItemMapLevel0)
//                    {
//                        m_graphicScene.setBackgroundBrush(QColor::fromRgba(qRgba(50, 50, 50, 255)));
//                        m_ptrGraphicsItemMapLevel0 = new QGraphicsPixmapItem(QPixmap::fromImage(*itLevel0));
//                        m_ptrGraphicsItemMapLevel1 = new QGraphicsPixmapItem(QPixmap::fromImage(*itLevel1));
//                        m_ptrGraphicsItemRectShadowTop = new QGraphicsRectItem();
//                        m_ptrGraphicsItemRectShadowLeft = new QGraphicsRectItem();
//                        m_ptrGraphicsItemRectShadowRight = new QGraphicsRectItem();
//                        m_ptrGraphicsItemRectShadowBottom = new QGraphicsRectItem();
//                        m_ptrGraphicsItemPixmapPlayerBottom = new QGraphicsPixmapItem();
//                        m_ptrGraphicsItemPixmapPlayerBottom->setPixmap(QPixmap(":/images/images/player.svg")
//                                                                 .scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//                        m_ptrGraphicsItemPixmapPlayerTop = new QGraphicsPixmapItem();
//                        m_ptrGraphicsItemPixmapPlayerTop->setPixmap(QPixmap(":/images/images/player.svg")
//                                                                 .scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//                        m_ptrGraphicsItemPixmapPlayerTop->setOpacity(0.5);

//                        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
//                        effect->setBlurRadius(20);
//                        effect->setOffset(0);
//                        effect->setColor(QColor(0, 0, 0, 255));
//                        m_ptrGraphicsItemPixmapPlayerTop->setGraphicsEffect(effect);

//                        m_graphicScene.addItem(m_ptrGraphicsItemMapLevel0);
//                        m_graphicScene.addItem(m_ptrGraphicsItemMapLevel1);
//                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowTop);
//                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowLeft);
//                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowRight);
//                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowBottom);
//                        m_graphicScene.addItem(m_ptrGraphicsItemPixmapPlayerBottom);
//                        m_graphicScene.addItem(m_ptrGraphicsItemPixmapPlayerTop);

//                        m_ptrGraphicsItemMapLevel1->setOpacity(0.8);

//                        ui->graphicsViewMap->setSceneRect(itLevel0->rect().adjusted(- itLevel0->rect().width(),
//                                                                                 - itLevel0->rect().height(),
//                                                                                 itLevel0->rect().width(),
//                                                                                 itLevel0->rect().height()));
//                        ui->graphicsViewMiniMap->setSceneRect(itLevel0->rect().adjusted(- itLevel0->rect().width(),
//                                                                                 - itLevel0->rect().height(),
//                                                                                 itLevel0->rect().width(),
//                                                                                 itLevel0->rect().height()));
//                    }
//                    else
//                    {
//                        m_ptrGraphicsItemMapLevel0->setPixmap(QPixmap::fromImage(*itLevel0));
//                        m_ptrGraphicsItemMapLevel1->setPixmap(QPixmap::fromImage(*itLevel1));
//                    }

//                    m_ptrGraphicsItemMapLevel0->setZValue(1);
//                    m_ptrGraphicsItemPixmapPlayerBottom->setZValue(2);
//                    m_ptrGraphicsItemMapLevel1->setZValue(3);
//                    m_ptrGraphicsItemPixmapPlayerTop->setZValue(4);

//                    m_ptrGraphicsItemMapLevel0->setX(0);
//                    m_ptrGraphicsItemMapLevel0->setY(0);
//                    m_ptrGraphicsItemMapLevel1->setX(0);
//                    m_ptrGraphicsItemMapLevel1->setY(0);

//                    ui->widgetMiniMap->setVisible(true);
//                }
//            }

//            QMap<QMUD::IdType, QPair<QGraphicsPixmapItem*, QMUD::ClientDataRoomInfo::ChInfo>> newChs;

//            for (auto& elem : data->chs())
//            {
//                auto it = m_mapChs.find(elem.id);

//                if (it == m_mapChs.end())
//                {
//                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem();
//                    //if (elem.race == QMUD::RaceType::KOBOLD)
//                    //    item->setPixmap(QPixmap(":/images/images/kobold.png").scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//                    //else if (elem.race == QMUD::RaceType::CAT)
//                    //    item->setPixmap(QPixmap(":/images/images/cat.png").scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//                    //else
//                        item->setPixmap(QPixmap(":/images/images/battle.svg").scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));

//                    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
//                    effect->setBlurRadius(10);
//                    effect->setOffset(0);
//                    effect->setColor(QColor(0, 0, 0, 255));

//                    item->setGraphicsEffect(effect);

//                    newChs[elem.id] = QPair<QGraphicsPixmapItem*, QMUD::ClientDataRoomInfo::ChInfo>(item, elem);

//                    m_graphicScene.addItem(item);
//                    item->setZValue(2);
//                }
//                else
//                {
//                    newChs[elem.id] = QPair<QGraphicsPixmapItem*, QMUD::ClientDataRoomInfo::ChInfo>(it.value().first, elem);
//                }
//            }

//            for (auto& elem : m_mapChs.keys())
//            {
//                if (!newChs.contains(elem))
//                {
//                    delete m_mapChs[elem].first;
//                }
//            }

//            m_mapChs = newChs;

//            for (auto& elem : m_mapChs)
//            {
//                elem.first->setX(elem.second.x * m_iTilesSizePixels + (m_iTilesSizePixels - elem.first->pixmap().width())/2);
//                elem.first->setY(elem.second.y * m_iTilesSizePixels + (m_iTilesSizePixels - elem.first->pixmap().height())/2);
//            }

//            QRect sceneRect((data->x() - m_iVisibleMapRadius) * m_iTilesSizePixels,
//                            (data->y() - m_iVisibleMapRadius) * m_iTilesSizePixels,
//                            m_iVisibleMapDiameter * m_iTilesSizePixels,
//                            m_iVisibleMapDiameter * m_iTilesSizePixels);

//            m_ptrGraphicsItemRectShadowTop->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
//            m_ptrGraphicsItemRectShadowBottom->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
//            m_ptrGraphicsItemRectShadowLeft->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
//            m_ptrGraphicsItemRectShadowRight->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
//            m_ptrGraphicsItemRectShadowTop->setPen(Qt::NoPen);
//            m_ptrGraphicsItemRectShadowBottom->setPen(Qt::NoPen);
//            m_ptrGraphicsItemRectShadowLeft->setPen(Qt::NoPen);
//            m_ptrGraphicsItemRectShadowRight->setPen(Qt::NoPen);
//            m_ptrGraphicsItemRectShadowTop->setZValue(1000);
//            m_ptrGraphicsItemRectShadowBottom->setZValue(1000);
//            m_ptrGraphicsItemRectShadowLeft->setZValue(1000);
//            m_ptrGraphicsItemRectShadowRight->setZValue(1000);

//            m_ptrGraphicsItemRectShadowTop->setRect(sceneRect.left(), sceneRect.top(), sceneRect.width()-1, -sceneRect.height()*1000);
//            m_ptrGraphicsItemRectShadowBottom->setRect(sceneRect.left(), sceneRect.bottom(), sceneRect.width()-1, sceneRect.height()*1000);
//            m_ptrGraphicsItemRectShadowLeft->setRect(sceneRect.left(), sceneRect.top()-sceneRect.height()*1000, -sceneRect.width()*1000, sceneRect.height()*3*1000);
//            m_ptrGraphicsItemRectShadowRight->setRect(sceneRect.right(), sceneRect.top()-sceneRect.height()*1000, sceneRect.width()*1000, sceneRect.height()*3*1000);

//            ui->graphicsViewMap->centerOn(sceneRect.center());


//            m_ptrGraphicsItemPixmapPlayerBottom->setPos(QPointF(sceneRect.center().x() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().width()/2,
//                                                                sceneRect.center().y() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().height()/2));
//            m_ptrGraphicsItemPixmapPlayerTop->setPos(QPointF(sceneRect.center().x() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().width()/2,
//                                                                sceneRect.center().y() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().height()/2));

//            ui->graphicsViewMiniMap->centerOn(sceneRect.center());
//        }
//        else if (pkt->type() == QMUD::ClientDataType::ROOM_CONTENT)
//        {
//            QMUD::ClientDataRoomContent *data = static_cast<QMUD::ClientDataRoomContent*>(pkt);

//            for (auto obj : ui->widgetRoomContent->children())
//                delete obj;

//            QVBoxLayout* layout = new QVBoxLayout();
//            for (auto ch : data->characters())
//            {
//                FormRoomContent* content = new FormRoomContent();
//                content->setMaximumHeight(60);
//                content->setNpcData(ch.name, ch.level, ch.hpCurrent, ch.hpMax, ch.mpCurrent, ch.mpMax, ch.id);
//                connect(content, SIGNAL(clicked()), this, SLOT(onRoomContentClicked()));
//                layout->addWidget(content);
//            }

//            layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

//            m_bClientDataRoomContentIsValid = true;
//            m_clientDataRoomContent = *data;

//            ui->widgetRoomContent->setLayout(layout);
//        }
//        else if (pkt->type() == QMUD::ClientDataType::NEW_COMBAT_ROUND)
//        {

//        }
//        else if (pkt->type() == QMUD::ClientDataType::ATTACK_INFO)
//        {
//            QMUD::ClientDataAttackInfo *data = static_cast<QMUD::ClientDataAttackInfo*>(pkt);

//            if (!m_bClientDataBasicInfoIsValid ||
//                    !m_bClientDataTargetInfoIsValid ||
//                    !m_bClientDataRoomContentIsValid)
//                return;

//            if (m_clientDataBasicInfo.id() == data->targetId())
//            {
//                if (data->isDamage())
//                    m_ptrDamageInfoPlayer->pushDamage(data->value());
//                else
//                    m_ptrDamageInfoPlayer->pushCure(data->value());
//            }
//            else if (m_clientDataTargetInfo.id() == data->targetId())
//            {
//                if (data->isDamage())
//                    m_ptrDamageInfoTarget->pushDamage(data->value());
//                else
//                    m_ptrDamageInfoTarget->pushCure(data->value());
//            }

//            pushAttackInfo(*data);
//        }
//        else if (pkt->type() == QMUD::ClientDataType::REQUEST)
//        {
//            QMUD::ClientDataRequest *data = static_cast<QMUD::ClientDataRequest*>(pkt);

//            ui->lineEdit->setEnabled(false);
//            ui->textBrowserRequest->clear();
//            appendEscapeSequenceToTextEdit(ui->textBrowserRequest, data->request());
//            ui->widgetRequest->setVisible(true);
//        }
//        else if (pkt->type() == QMUD::ClientDataType::CDT_MESSAGE)
//        {
//            QMUD::ClientDataMessage *data = static_cast<QMUD::ClientDataMessage*>(pkt);

//            switch (data->message())
//            {
//            case QMUD::ClientDataMessage::Message::ERROR_GENERIC:

//                break;

//            case QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED:
//            case QMUD::ClientDataMessage::Message::ERROR_SIGNIN_FAILED:
//                ui->labelLoginLoginFailed->setVisible(true);
//                break;

//            case QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED_CONTACT_ADMINISTRATOR:
//                ui->labelLoginLoginFailed->setVisible(true);
//                ui->labelLoginContactAdmin->setVisible(true);
//                break;

//            case QMUD::ClientDataMessage::Message::ERROR_USERNAME_FAILED:
//            case QMUD::ClientDataMessage::Message::ERROR_USERNAME_EXISTS:
//                ui->labelLoginUsernameFailed->setVisible(true);
//                break;

//            case QMUD::ClientDataMessage::Message::ERROR_PASSWORD_FAILED:
//                ui->labelLoginPasswordFailed->setVisible(true);
//                break;

//            case QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED: break;
//            case QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED_CONTACT_ADMINISTRATOR: break;
//            case QMUD::ClientDataMessage::Message::ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR: break;
//            case QMUD::ClientDataMessage::Message::ERROR_CH_NAME_FAILED:
//                break;

//            case QMUD::ClientDataMessage::Message::INFO_SIGNIN_OK:
//                ui->labelLoginSigninOk->setVisible(true);
//                break;

//            case QMUD::ClientDataMessage::Message::INFO_CH_CREATION_OK: break;

//            case QMUD::ClientDataMessage::Message::CONN_STATUS_IDLE:
//            case QMUD::ClientDataMessage::Message::CONN_STATUS_LOGIN:
//                ui->stackedWidget->setCurrentWidget(ui->pageLogin);
//                ui->stackedWidget->setCurrentWidget(ui->pageLogin);
//                break;

//            case QMUD::ClientDataMessage::Message::CONN_STATUS_CHARACTERS_LIST:
//                ui->stackedWidget->setCurrentWidget(ui->pageCharactersList);
//                break;

//            case QMUD::ClientDataMessage::Message::CONN_STATUS_IN_GAME:
//                ui->stackedWidget->setCurrentWidget(ui->pageInGame);
//                break;
//            }

//            emit messageReceived(data->message());
//        }
//        else if (pkt->type() == QMUD::ClientDataType::CHARACTERS_LIST)
//        {
//            QMUD::ClientDataCharactersList *data = static_cast<QMUD::ClientDataCharactersList*>(pkt);

//            ui->listWidgetChList->clear();

//            auto list = data->chsList();

//            for (auto elem : list)
//            {
//                QListWidgetItem* item = new QListWidgetItem();
//                ui->listWidgetChList->addItem(item);

//                FormCharacterInfo* wdg = new FormCharacterInfo();
//                wdg->setData(elem);
//                connect(wdg, &FormCharacterInfo::connect, this, &MainWindow::onCharacterConnectClicked);

//                ui->listWidgetChList->setItemWidget(item, wdg);

//                wdg->adjustSize();
//                item->setSizeHint(wdg->sizeHint() + QSize(0, 15));
//            }
//        }

        delete pkt;
    }
    else
    {
//        QJsonObject obj = QJsonDocument::fromJson(json).object();

//        QString type = obj["t"].toString();

//        if (type == "5") // Chat
//        {
//            enum class SayType
//            {
//                SAY_CHARACTER,
//                SAY_GROUP,
//                SAY_ROOM,
//                SAY_AREA
//            };

//            SayType sayType = static_cast<SayType>(obj["s"].toInt());
//            QString from = obj["fn"].toString();
//            QString to = obj["tn"].toString();
//            QString message = obj["m"].toString();

//            if (sayType == SayType::SAY_CHARACTER)
//            {
//                if (from.toLower() != ui->labelChName->text().toLower())
//                    ui->textBrowserChat->append(QString("<font color=\"pink\">[</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"pink\">]:%2</font>")
//                                                .arg(from)
//                                                .arg(message));
//                else
//                    ui->textBrowserChat->append(QString("<font color=\"pink\">[</font><font color=\"white\">tu a <a href=\"dico a '%1'\" style=\"color:white\">%1</a></font><font color=\"pink\">]:%2</font>")
//                                                .arg(to)
//                                                .arg(message));
//            }
//            else if (sayType == SayType::SAY_GROUP)
//            {
//                if (from.toLower() != ui->labelChName->text().toLower())
//                    ui->textBrowserChat->append(QString("<font color=\"#2ECCFA\">[<a href=\"gdico\" style=\"color:white\">GRUPPO</a>:</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"#2ECCFA\">]:%2</font>")
//                                                .arg(from)
//                                                .arg(message));
//                else
//                    ui->textBrowserChat->append(QString("<font color=\"#2ECCFA\">[<a href=\"gdico\" style=\"color:white\">GRUPPO</a>:</font><font color=\"white\">tu</font><font color=\"#2ECCFA\">]:%1</font>")
//                                                .arg(message));
//            }
//            else if (sayType == SayType::SAY_ROOM)
//            {
//                if (from.toLower() != ui->labelChName->text().toLower())
//                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"sdico\" style=\"color:white\">STANZA</a>:</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"white\">]:%2</font>")
//                                                .arg(from)
//                                                .arg(message));
//                else
//                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"sdico\" style=\"color:white\">STANZA</a>:</font><font color=\"white\">tu</font><font color=\"white\">]:%1</font>")
//                                                .arg(message));
//            }
//            else if (sayType == SayType::SAY_AREA)
//            {
//                if (from.toLower() != ui->labelChName->text().toLower())
//                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"adico\" style=\"color:white\">AREA</a>:</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"white\">]:%2</font>")
//                                                .arg(from)
//                                                .arg(message));
//                else
//                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"adico\" style=\"color:white\">AREA</a>:</font><font color=\"white\">tu</font><font color=\"white\">]:%1</font>")
//                                                .arg(message));
//            }
//        }
    }
}

bool ConnectionHandler::isConnected() const
{
    return m_bIsConnected;
}

ConnectionHandler::PageType ConnectionHandler::currentPage() const
{
    return m_eCurrentPage;
}

QString ConnectionHandler::lastErrorMessage() const
{
    return m_strLastErrorMessage;
}

QString ConnectionHandler::lastInfoMessage() const
{
    return m_strLastInfoMessage;
}

QPoint ConnectionHandler::playerPosition() const
{
    return m_ptPlayerPosition;
}

QPoint ConnectionHandler::playerMapId() const
{
    return m_ptPlayerMapId;
}

QString ConnectionHandler::playerName() const
{
    return m_strPlayerName;
}

int ConnectionHandler::playerLevel() const
{
    return m_iPlayerLevel;
}

QString ConnectionHandler::playerClass() const
{
    return m_strPlayerClass;
}

QString ConnectionHandler::playerRace() const
{
    return m_strPlayerRace;
}

int ConnectionHandler::playerExperience() const
{
    return m_iPlayerExperience;
}

int ConnectionHandler::playerExperienceToLevelUp() const
{
    return m_iPlayerExperienceToLevelUp;
}

int ConnectionHandler::playerHpCurrent() const
{
    return m_iPlayerHpCurrent;
}

int ConnectionHandler::playerHpMaximum() const
{
    return m_iPlayerHpMaximum;
}

int ConnectionHandler::playerMpCurrent() const
{
    return m_iPlayerMpCurrent;
}

int ConnectionHandler::playerMpMaximum() const
{
    return m_iPlayerMpMaximum;
}

int ConnectionHandler::playerApCurrent() const
{
    return m_iPlayerApCurrent;
}

int ConnectionHandler::playerApMaximum() const
{
    return m_iPlayerApMaximum;
}

QMUD::IdType ConnectionHandler::selectedPcId() const
{
    return m_iSelectedPcId;
}

QString ConnectionHandler::selectedPcName() const
{
    return m_strSelectedPcName;
}

int ConnectionHandler::selectedPcLevel() const
{
    return m_iSelectedPcLevel;
}

QString ConnectionHandler::selectedPcClass() const
{
    return m_strSelectedPcClass;
}

QString ConnectionHandler::selectedPcRace() const
{
    return m_strSelectedPcRace;
}

int ConnectionHandler::selectedPcHpCurrent() const
{
    return m_iSelectedPcHpCurrent;
}

int ConnectionHandler::selectedPcHpMaximum() const
{
    return m_iSelectedPcHpMaximum;
}

int ConnectionHandler::selectedPcMpCurrent() const
{
    return m_iSelectedPcMpCurrent;
}

int ConnectionHandler::selectedPcMpMaximum() const
{
    return m_iSelectedPcMpMaximum;
}

int ConnectionHandler::selectedPcApCurrent() const
{
    return m_iSelectedPcApCurrent;
}

int ConnectionHandler::selectedPcApMaximum() const
{
    return m_iSelectedPcApMaximum;
}


QStringList ConnectionHandler::npcs() const
{
    return m_vNpcs;
}

