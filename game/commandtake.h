/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDTAKE_H
#define COMMANDTAKE_H

#include "command.h"

namespace CORE
{

class CommandTake : public Command
{
    friend class Command;
    friend class CommandTakeHandler;

public:
    CommandTake();
    virtual ~CommandTake();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    bool takeItem(Character* ch, Player *player, QList<Character *> characters, Item* targetItem);
};

} // namespace CORE

namespace CORE
{

class CommandTakeHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandTake::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandTake::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandTake::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandTake());
    }
};

} // namespace CORE

#endif // COMMANDTAKE_H
