/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDTAKEOFF_H
#define COMMANDTAKEOFF_H

#include "command.h"

namespace CORE
{

class CommandTakeOff : public Command
{
    friend class Command;
    friend class CommandTakeOffHandler;

public:
    CommandTakeOff();
    virtual ~CommandTakeOff();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    bool takeOffItem(Character *ch, Player *player, QList<Character *> characters, Item *targetItem);
};

} // namespace CORE

namespace CORE
{

class CommandTakeOffHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandTakeOff::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandTakeOff::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandTakeOff::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandTakeOff());
    }
};

} // namespace CORE

#endif // COMMANDTAKEOFF_H
