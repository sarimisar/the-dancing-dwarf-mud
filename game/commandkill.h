/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDKILL_H
#define COMMANDKILL_H

#include "command.h"

namespace CORE
{

class CommandKill : public Command
{
    friend class Command;
    friend class CommandKillHandler;

public:
    CommandKill();
    virtual ~CommandKill();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);
};

} // namespace CORE

namespace CORE
{

class CommandKillHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandKill::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandKill::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandKill::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandKill());
    }
};

} // namespace CORE

#endif // COMMANDKILL_H
