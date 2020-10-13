/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDDROP_H
#define COMMANDDROP_H

#include "command.h"

namespace CORE
{

class CommandDrop : public Command
{
    friend class Command;
    friend class CommandDropHandler;

public:
    CommandDrop();
    virtual ~CommandDrop();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    bool dropItem(Character* ch, Player *player, QList<Character *> characters, Item *targetItem);
};

} // namespace CORE

namespace CORE
{

class CommandDropHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandDrop::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandDrop::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandDrop::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandDrop());
    }
};

} // namespace CORE

#endif // COMMANDDROP_H
