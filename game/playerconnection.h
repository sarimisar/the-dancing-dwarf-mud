/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef PLAYERCONNECTION_H
#define PLAYERCONNECTION_H

#include <QObject>
#include <QDateTime>

#include "../global.h"

namespace NET
{
    class NetConnection;
}

namespace CORE
{

class Player;

class PlayerConnection : public QObject
{
    Q_OBJECT

public:
    enum class PlayerConnectionStatus
    {
        PLAYER_CONN_STATUS_IDLE,
        PLAYER_CONN_STATUS_LOGIN,
        PLAYER_CONN_STATUS_CHARACTERS_LIST,
        PLAYER_CONN_STATUS_IN_GAME
    };

    explicit PlayerConnection(NET::NetConnection* conn, QObject *parent = Q_NULLPTR);
    virtual ~PlayerConnection();

    QMUD::IdType id() const;

    QString address() const;

    void close();

    void setPlayerConnectionStatus(PlayerConnectionStatus status);
    PlayerConnectionStatus playerConnectionStatus() const;

    void write(QString str, bool colorConversion = true);
    void writeData(QByteArray data);

    Player *player() const;
    void setPlayer(Player *player);

    void sendMessage(QMUD::ClientDataMessage::Message message);

    bool checkConnection();

    bool isCustomClient() const;

signals:
    void disconnected(PlayerConnection *conn, Player* player);
    void commandReceived(PlayerConnection *conn, Player* player, QString cmd);

public slots:

private slots:
    void onConnectionDisconnected();
    void onCommandReceived(QString cmd);

private:
    bool isConnected() const;

private:
    QMUD::IdType m_id;

    NET::NetConnection *m_ptrNetConnection;
    PlayerConnectionStatus m_ePlayerConnectionStatus;
    QString m_strAddress;

    int m_iCommandsCounter;
    QDateTime m_timeLastCommand;

    Player *m_ptrPlayer;
};

} // namespace CORE

#endif // PLAYERCONNECTION_H
