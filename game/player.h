/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "playerconnection.h"
#include "pccharacter.h"
#include "global.h"

#include <QObject>
#include <QDir>

namespace CORE
{

class Player : public QObject
{
    Q_OBJECT

public:
    enum class PlayerLevel
    {
        PLAYER_LEVEL_NORMAL = 0,
        PLAYER_LEVEL_ADMIN = 100,
        PLAYER_LEVEL_CREATOR = 200
    };

    explicit Player(PlayerConnection* conn, QObject *parent = Q_NULLPTR);
    virtual ~Player();

    static Player* loadPlayer(QDir rootDir, PlayerConnection* conn, QString username, QString password, QMUD::FileOpStatus &status, QObject* parent);
    static bool createPlayer(QDir rootDir, PlayerConnection* conn, QString username, QString password, QMUD::FileOpStatus &status);
    static bool exists(QDir rootDir, QString username);

    void forceSave();
    void closeConnection();

    QStringList charactersList() const;

    void addCharacterToList(QString character);

    PcCharacter *loadCharacter(QDir rootDir, QString name, QMUD::FileOpStatus &status, Room *parent);

    void setPcCharacter(PcCharacter* ch);
    PcCharacter* pcCharacter() const;

    PlayerConnection* connection() const;
    QString username() const;
    QString usernameBeautify() const;

    void send(QString str);
    void sendLine(QString str);
    void sendNoColorConversion(QString str);
    void sendLineNoColorConversion(QString str);
    void sendCharactersList();
    void sendStatus();
    void sendData(QByteArray data);

    void sendConfirmationRequest(QString command);
    bool waitConfirmation() const;
    QString clearConfirmationRequest();

    PlayerLevel level() const;
    void setLevel(PlayerLevel level);

    bool isVisible() const;

signals:
    void update();

public slots:

private slots:
    void onUpdate();

private:
    bool readJson(const QJsonObject& obj);
    void writeJson(QJsonObject &obj);
    void save();

private:
    PlayerLevel m_ePlayerLevel;

    PlayerConnection* m_ptrConnection;
    PcCharacter* m_ptrPcCharacter;

    QString m_strFileName;

    QString m_strUsername;
    QString m_strPassword;
    QStringList m_listCharacters;

    QString m_strConfirmationRequestCommand;
};

} // namespace CORE

#endif // PLAYER_H
