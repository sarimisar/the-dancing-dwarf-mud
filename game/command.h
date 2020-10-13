/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMAND_H
#define COMMAND_H

#include "character.h"
#include "pccharacter.h"
#include "player.h"

namespace CORE
{

class Command
{
public:
    typedef QSharedPointer<Command> Ptr;

    virtual ~Command();

    static Ptr parse(QString str, Character* ch);

    void execute(bool confirmation = false);
    Player::PlayerLevel minPlayerLevel() const;

protected:
    Command();
    static bool isOf(QString cmd, QString str);

    virtual void executePc(Character* ch, Player* player) = 0;
    virtual void executePcConfirmation(Character* ch, Player* player) { Q_UNUSED(ch) Q_UNUSED(player) }
    virtual void executeNpc(Character* ch) = 0;

    QString command() const;
    QStringList args() const;

    Entity *findTargetInRoom(QString str, bool &wrongUsage, bool visibleOnly);
    Character *findCharacterInMap(QString str, bool &wrongUsage, bool visibleOnly);
    Item *findItemInInventory(QString str, QVector<Item*> inventory, bool &wrongUsage);
    Character *findCharacterInRoom(QMUD::IdType chId, bool visibleOnly);

private:
    void init(QString command, QStringList args, Character* ch);

protected:
    static bool splitCommandArgs(QStringList& qargs, QString command, int maxArgs = -1);

private:
    Player::PlayerLevel m_eMinPlayerLevel;

    QString m_strCommand;
    QStringList m_strArgs;
    Character* m_ptrCh;
};

class CommandHandler
{
public:
    typedef QSharedPointer<CommandHandler> Ptr;

    virtual ~CommandHandler()
    {

    }

    virtual bool isOf(QString cmd, QVector<QVariant>& args) = 0;
    virtual bool sendUsage(Player* player, QStringList args) = 0;
    virtual Command::Ptr create(Player* player, QVector<QVariant> args) = 0;
    virtual QStringList commands(Player* player) = 0;
};

class CommandNull : public Command
{
public:
    CommandNull(QString strCmd = QString()) : Command(), m_strCmd(strCmd) { }
    virtual ~CommandNull() { }

protected:
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    QString m_strCmd;
};

}

#endif // COMMAND_H
