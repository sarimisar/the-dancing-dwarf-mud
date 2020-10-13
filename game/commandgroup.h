/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDGROUP_H
#define COMMANDGROUP_H

#include "command.h"

namespace CORE
{

class CommandGroup : public Command
{
    friend class Command;
    friend class CommandGroupHandler;

public:
    CommandGroup();
    virtual ~CommandGroup();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);
};

} // namespace CORE

namespace CORE
{

class CommandGroupHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandGroup::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandGroup::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandGroup::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandGroup());
    }
};

} // namespace CORE

#endif // COMMANDGROUP_H
