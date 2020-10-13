/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDLOOT_H
#define COMMANDLOOT_H

#include "command.h"

namespace CORE
{

class CommandLoot : public Command
{
    friend class Command;
    friend class CommandLootHandler;

public:
    CommandLoot();
    virtual ~CommandLoot();

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

class CommandLootHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandLoot::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandLoot::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandLoot::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandLoot());
    }
};

} // namespace CORE

#endif // COMMANDLOOT_H
