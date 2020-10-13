/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "player.h"
#include "playerconnection.h"
#include "character.h"
#include "pccharacter.h"
#include "world.h"
#include "../global.h"
#include "../globaldebug.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QCryptographicHash>

using namespace CORE;

Player::Player(PlayerConnection *conn, QObject *parent) :
    QObject(parent),
    m_ePlayerLevel(PlayerLevel::PLAYER_LEVEL_NORMAL),
    m_ptrConnection(conn),
    m_ptrPcCharacter(Q_NULLPTR)
{
    connect(this, SIGNAL(update()), this, SLOT(onUpdate()), Qt::QueuedConnection);
}

Player::~Player()
{
    GLOBAL_DEBUG_DESTRUCTOR("Player", m_strUsername);
}

void Player::onUpdate()
{
    save();
}

Player *Player::loadPlayer(QDir rootDir, PlayerConnection *conn, QString username, QString password, QMUD::FileOpStatus &status, QObject *parent)
{
    QString playerFilePath = rootDir.path() + "/" + username.toLower() + ".json";

    if (!QFileInfo(playerFilePath).exists())
    {
        status = QMUD::FileOpStatus::FILE_OP_ERROR_INVALID_NAME;
        return Q_NULLPTR;
    }

    QFile fp(playerFilePath);
    if (!fp.open(QFile::ReadOnly | QFile::Text))
    {
        GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(playerFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_OPEN_FILE;
        return Q_NULLPTR;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(fp.readAll(), &error);
    fp.close();

    if (error.error != QJsonParseError::NoError)
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: %2")
                        .arg(playerFilePath)
                        .arg(error.errorString()));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return Q_NULLPTR;
    }

    QJsonObject obj = doc.object();

    auto itPassword = obj.find("password");
    if (itPassword == obj.end())
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'password' non presente")
                        .arg(playerFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return Q_NULLPTR;
    }

    password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256).toHex();

    if (itPassword.value().toString() != password)
    {
        status = QMUD::FileOpStatus::FILE_OP_ERROR_GENERIC;
        return Q_NULLPTR;
    }

    Player *p = new Player(conn);
    p->setParent(parent);
    if (!p->readJson(obj))
    {
        delete p;
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1")
                        .arg(playerFilePath));
        return Q_NULLPTR;
    }

    status = QMUD::FileOpStatus::FILE_OP_OK;
    p->m_strFileName = playerFilePath;
    return p;
}

bool Player::createPlayer(QDir rootDir, PlayerConnection *conn, QString username, QString password, QMUD::FileOpStatus &status)
{
    QString playerFilePath = rootDir.path() + "/" + username.toLower() + ".json";

    if (QFileInfo(playerFilePath).exists())
    {
        status = QMUD::FileOpStatus::FILE_OP_ERROR_FILE_ALREADY_EXISTS;
        return false;
    }

    Player pTmp(conn, Q_NULLPTR);
    pTmp.m_strUsername = username.toLower();
    pTmp.m_strPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256).toHex();

    QFile fp(playerFilePath);
    if (!fp.open(QFile::WriteOnly | QFile::Text))
    {
        GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(playerFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_OPEN_FILE;
        return false;
    }

    QJsonObject obj;
    pTmp.writeJson(obj);
    QJsonDocument doc;
    doc.setObject(obj);
    QTextStream stream(&fp);
    stream << doc.toJson(QJsonDocument::Indented);
    fp.close();

    status = QMUD::FileOpStatus::FILE_OP_OK;
    return true;
}

bool Player::exists(QDir rootDir, QString username)
{
    QString playerFilePath = rootDir.path() + username + ".json";

    return QFileInfo(playerFilePath).exists();
}

void Player::forceSave()
{
    save();
}

void Player::closeConnection()
{
    m_ptrConnection->close();
}

QStringList Player::charactersList() const
{
    return m_listCharacters;
}

void Player::addCharacterToList(QString character)
{
    if (m_listCharacters.contains(character.toLower()))
        GLOBAL_BUG(tr("Character '%1' gia' presente nella lista del player '%2'").arg(character).arg(m_strUsername));
    else
    {
        m_listCharacters.push_back(character);
    }
}

void Player::save()
{
    if (m_strFileName.isEmpty())
        return;

    QString playerFilePath = m_strFileName;

    if (!QFileInfo(playerFilePath).exists())
    {
        GLOBAL_BUG(tr("Il file '%1' non esiste").arg(playerFilePath));
        m_ptrConnection->close();
    }

    QFile fp(playerFilePath);
    if (!fp.open(QFile::WriteOnly | QFile::Text))
    {
        GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1 durante il salvataggio").arg(playerFilePath));
    }

    QJsonObject obj;
    writeJson(obj);
    QJsonDocument doc;
    doc.setObject(obj);
    QTextStream stream(&fp);
    stream << doc.toJson(QJsonDocument::Indented);
    fp.close();

    if (m_ptrPcCharacter)
        m_ptrPcCharacter->save();
}

PcCharacter *Player::loadCharacter(QDir rootDir, QString name, QMUD::FileOpStatus &status, Room* parent)
{
    QRegExp re("\\d*");

    if (re.exactMatch(name))
    {
        int index = name.toInt();

        if (index < 0 || index >= m_listCharacters.size())
        {
            status = QMUD::FileOpStatus::FILE_OP_ERROR_INVALID_NAME;
            return Q_NULLPTR;
        }
        else
            name = m_listCharacters[index];
    }

    if (m_listCharacters.contains(name))
        return PcCharacter::loadCharacter(rootDir, m_ptrConnection, this, name, status, parent);
    else
    {
        status = QMUD::FileOpStatus::FILE_OP_ERROR_INVALID_NAME;
        return Q_NULLPTR;
    }
}

void Player::setPcCharacter(PcCharacter *ch)
{
    if (m_ptrPcCharacter && ch)
        Q_ASSERT(false);

    m_ptrPcCharacter = ch;
}

PcCharacter *Player::pcCharacter() const
{
    return m_ptrPcCharacter;
}

PlayerConnection *Player::connection() const
{
    return m_ptrConnection;
}

QString Player::username() const
{
    return m_strUsername;
}

QString Player::usernameBeautify() const
{
    QString name = m_strUsername;
    name[0] = name[0].toUpper();
    return name;
}

void Player::send(QString str)
{
    m_ptrConnection->write(str);
}

void Player::sendLine(QString str)
{
    m_ptrConnection->write(str + QMUD_GLOBAL_NEW_LINE);
}

void Player::sendNoColorConversion(QString str)
{
    m_ptrConnection->write(str, false);
}

void Player::sendLineNoColorConversion(QString str)
{
    m_ptrConnection->write(str + QMUD_GLOBAL_NEW_LINE, false);
}

void Player::sendCharactersList()
{
    QVector<QMUD::PcCharacterBaseInfo> list;
    for (QString ch : m_listCharacters)
    {
        auto it = QMUD::qmudWorld->pcCharactersBaseInfo().find(ch.toLower());
        if (it == QMUD::qmudWorld->pcCharactersBaseInfo().end())
            continue;

        list.push_back(it.value());
    }

    QMUD::ClientDataCharactersList data(list);
    sendData(data.serialize());
}

void Player::sendStatus()
{
    Q_ASSERT(m_ptrPcCharacter);

    if (connection()->playerConnectionStatus() != PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_IN_GAME)
        return;

    m_ptrPcCharacter->sendStatus();
}

void Player::sendData(QByteArray data)
{
    m_ptrConnection->writeData(data);
}

void Player::sendConfirmationRequest(QString command)
{
    m_strConfirmationRequestCommand = command;
}

bool Player::waitConfirmation() const
{
    return !m_strConfirmationRequestCommand.isEmpty();
}

QString Player::clearConfirmationRequest()
{
    QString tmp = m_strConfirmationRequestCommand;
    m_strConfirmationRequestCommand.clear();
    return tmp;
}

Player::PlayerLevel Player::level() const
{
    return m_ePlayerLevel;
}

void Player::setLevel(Player::PlayerLevel level)
{
    m_ePlayerLevel = level;
}

bool Player::isVisible() const
{
    return true; //!(m_ePlayerLevel >= PlayerLevel::PLAYER_LEVEL_ADMIN);
}

bool Player::readJson(const QJsonObject &obj)
{
    int tmpInt;

    // playerLevel
    if (!QMUD::fileJsonRead(obj, "playerLevel", tmpInt, true, static_cast<int>(PlayerLevel::PLAYER_LEVEL_NORMAL)))
        return false;

    if (tmpInt == static_cast<int>(PlayerLevel::PLAYER_LEVEL_CREATOR))
        m_ePlayerLevel = PlayerLevel::PLAYER_LEVEL_CREATOR;
    else if (tmpInt == static_cast<int>(PlayerLevel::PLAYER_LEVEL_ADMIN))
        m_ePlayerLevel = PlayerLevel::PLAYER_LEVEL_ADMIN;
    else
        m_ePlayerLevel = PlayerLevel::PLAYER_LEVEL_NORMAL;

    // Username
    if (!QMUD::fileJsonRead(obj, "username", m_strUsername))
        return false;

    m_strUsername = m_strUsername.toLower();

    // Password
    if (!QMUD::fileJsonRead(obj, "password", m_strPassword))
        return false;

    // Characters
    if (!QMUD::fileJsonRead(obj, "characters", m_listCharacters, true, true))
        return false;

    return true;
}

void Player::writeJson(QJsonObject &obj)
{
    obj["playerLevel"] = static_cast<int>(m_ePlayerLevel);
    obj["username"] = m_strUsername;
    obj["password"] = m_strPassword;
    obj["characters"] = QJsonArray::fromStringList(m_listCharacters);
}
