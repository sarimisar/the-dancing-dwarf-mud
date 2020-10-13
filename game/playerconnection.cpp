/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "playerconnection.h"
#include "../global.h"
#include "../globaldebug.h"
#include "../network/netconnection.h"
#include "color.h"

using namespace CORE;

PlayerConnection::PlayerConnection(NET::NetConnection *conn, QObject *parent) :
    QObject(parent),
    m_ptrNetConnection(conn),
    m_ePlayerConnectionStatus(PlayerConnectionStatus::PLAYER_CONN_STATUS_IDLE),
    m_iCommandsCounter(0),
    m_timeLastCommand(QDateTime::currentDateTime()),
    m_ptrPlayer(Q_NULLPTR)
{
    Q_ASSERT(!!conn);

    static QMUD::IdType id = 0;
    m_id = id++;

    connect(m_ptrNetConnection, SIGNAL(disconnected()), this, SLOT(onConnectionDisconnected()));
    connect(m_ptrNetConnection, SIGNAL(commandReceived(QString)), this, SLOT(onCommandReceived(QString)));

    m_strAddress = conn->connectionAddress();
}

PlayerConnection::~PlayerConnection()
{
    GLOBAL_DEBUG_DESTRUCTOR("PlayerConnection", m_strAddress);
}

QMUD::IdType PlayerConnection::id() const
{
    return m_id;
}

void PlayerConnection::close()
{
    setPlayerConnectionStatus(PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_IDLE);

    if (isConnected())
    {
        GLOBAL_INFO(tr("Chiusura forzata della connessione %1")
                    .arg(m_ptrNetConnection->connectionAddress()));
        m_ptrNetConnection->disconnectFromHost();
    }
    else
    {
        GLOBAL_CRITICAL(tr("Chiusura forzata della connessione %1; connessione gia' sconnessa")
                       .arg(m_ptrNetConnection->connectionAddress()));
    }
}

QString PlayerConnection::address() const
{
    if (!isConnected())
        return tr("%1 (disconnesso)").arg(m_strAddress);

    return m_ptrNetConnection->connectionAddress();
}

void PlayerConnection::setPlayerConnectionStatus(PlayerConnection::PlayerConnectionStatus status)
{
    m_ePlayerConnectionStatus = status;

    if (status == PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_IDLE)
        sendMessage(QMUD::ClientDataMessage::Message::CONN_STATUS_IDLE);
    else if (status == PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_LOGIN)
        sendMessage(QMUD::ClientDataMessage::Message::CONN_STATUS_LOGIN);
    else if (status == PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_CHARACTERS_LIST)
        sendMessage(QMUD::ClientDataMessage::Message::CONN_STATUS_CHARACTERS_LIST);
    else if (status == PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_IN_GAME)
        sendMessage(QMUD::ClientDataMessage::Message::CONN_STATUS_IN_GAME);
    else
        Q_ASSERT(false);
}

PlayerConnection::PlayerConnectionStatus PlayerConnection::playerConnectionStatus() const
{
    return m_ePlayerConnectionStatus;
}

void PlayerConnection::write(QString str, bool colorConversion)
{
    if (!isConnected())
        return;

    if (colorConversion)
        m_ptrNetConnection->send(Color::strEscapeColorsConversion(str, true, isCustomClient()));
    else
        m_ptrNetConnection->send(str);
}

void PlayerConnection::writeData(QByteArray data)
{
    if (!isConnected())
        return;

    m_ptrNetConnection->sendData(data);
}

Player *PlayerConnection::player() const
{
    return m_ptrPlayer;
}

void PlayerConnection::setPlayer(Player *player)
{
    if (!m_ptrPlayer)
        m_ptrPlayer = player;
    else
    {
        Q_ASSERT(false);
        GLOBAL_BUG("Chiamata a setPlayer con m_ptrPlayer non NULL");
    }
}

void PlayerConnection::sendMessage(QMUD::ClientDataMessage::Message message)
{
    QMUD::ClientDataMessage data(message);
    writeData(data.serialize());
}

bool PlayerConnection::checkConnection()
{
    QDateTime now = QDateTime::currentDateTime();

    if (m_timeLastCommand.addMSecs(100) > now)
        m_iCommandsCounter++;
    else
        m_iCommandsCounter = 0;

    if (m_iCommandsCounter > 20)
    {
        close();
        return false;
    }

    m_timeLastCommand = now;
    return true;
}

bool PlayerConnection::isCustomClient() const
{
    return m_ptrNetConnection->isCustomClient();
}

void PlayerConnection::onConnectionDisconnected()
{
    //GLOBAL_INFO(tr("Chiusura della connessione %1").arg(m_ptrNetConnection->connectionAddress()));

    m_ptrNetConnection = Q_NULLPTR;

    emit disconnected(this, m_ptrPlayer);
}

void PlayerConnection::onCommandReceived(QString cmd)
{
    if (!isConnected())
        return;

    emit commandReceived(this, m_ptrPlayer, cmd);
}

bool PlayerConnection::isConnected() const
{
    return !!m_ptrNetConnection;
}
