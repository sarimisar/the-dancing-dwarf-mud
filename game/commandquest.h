/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDQUEST_H
#define COMMANDQUEST_H

#include "command.h"

namespace CORE
{

class CommandQuest : public Command
{
    friend class Command;
    friend class CommandQuestHandler;

public:
    enum class ActionType
    {
        QUEST,
        DIARY
    };

    CommandQuest(ActionType action);
    virtual ~CommandQuest();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    ActionType m_eActionType;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandQuest::ActionType)

namespace CORE
{

class CommandQuestHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandQuest::ActionType action;
        bool ret = CommandQuest::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandQuest::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandQuest::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandQuest(args[0].value<CommandQuest::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDQUEST_H
