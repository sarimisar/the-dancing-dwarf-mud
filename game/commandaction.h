/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDACTION_H
#define COMMANDACTION_H

#include "command.h"
#include "roomexit.h"

namespace CORE
{

class CommandAction : public Command
{
    friend class Command;
    friend class CommandActionHandler;

public:
    CommandAction(QMUD::ActionType action);
    virtual ~CommandAction();

protected:
    static bool isOf(QString cmd, QMUD::ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    void toggleExit(Character *ch, Player* player, RoomExit& exit);

private:
    QMUD::ActionType m_eAction;
};

} // namespace CORE

namespace CORE
{

class CommandActionHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        QMUD::ActionType action;
        bool ret = CommandAction::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandAction::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandAction::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandAction(args[0].value<QMUD::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDACTION_H
