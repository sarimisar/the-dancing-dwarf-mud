/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDEAT_H
#define COMMANDEAT_H

#include "command.h"

namespace CORE
{

class CommandEat : public Command
{
    friend class Command;
    friend class CommandEatHandler;

public:
    enum class ActionType
    {
        EAT,
        DRINK
    };

    CommandEat(ActionType action);
    virtual ~CommandEat();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    ActionType m_eAction;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandEat::ActionType)

namespace CORE
{

class CommandEatHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandEat::ActionType action;
        bool ret = CommandEat::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandEat::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandEat::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandEat(args[0].value<CommandEat::ActionType>()));
    }
};

} // namespace CORE


#endif // COMMANDEAT_H
