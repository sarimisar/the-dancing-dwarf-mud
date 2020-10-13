/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDEMPTY_H
#define COMMANDEMPTY_H

#include "command.h"

namespace CORE
{

class CommandEmpty : public Command
{
    friend class Command;
    friend class CommandEmptyHandler;

public:
    CommandEmpty();
    virtual ~CommandEmpty();

protected:
    static bool isOf(QString cmd);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);
};

} // namespace CORE


namespace CORE
{

class CommandEmptyHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandEmpty::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return QStringList();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return false;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandEmpty());
    }
};

} // namespace CORE


#endif // COMMANDEMPTY_H
