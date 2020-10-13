/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDRESOURCES_H
#define COMMANDRESOURCES_H

#include "command.h"

#include <QVariant>

namespace CORE
{

class CommandResources : public Command
{
    friend class Command;
    friend class CommandResourcesHandler;

public:
    enum class ActionType
    {
        LIST,
        RADAR,
        MINING,
        ERBALISM
    };

    CommandResources(ActionType action);
    virtual ~CommandResources();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    ActionType m_eAction;
};

}

Q_DECLARE_METATYPE(CORE::CommandResources::ActionType)

namespace CORE
{

class CommandResourcesHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandResources::ActionType action;
        bool ret = CommandResources::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandResources::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandResources::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandResources(args[0].value<CommandResources::ActionType>()));
    }
};

} // namespace CORE



#endif // COMMANDRESOURCES_H
