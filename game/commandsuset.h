/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSUSET_H
#define COMMANDSUSET_H

#include "command.h"

namespace CORE
{

class CommandSuSet : public Command
{
    friend class Command;
    friend class CommandSuSetHandler;

public:
    enum class ActionType
    {
        LEVELUP,
        SET
    };

    CommandSuSet(ActionType action);
    virtual ~CommandSuSet();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    void showResourceList(Player* player);
    void sendUsageSuSetResource(Player* player);

private:
    ActionType m_eAction;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandSuSet::ActionType)

namespace CORE
{

class CommandSuSetHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandSuSet::ActionType action;
        bool ret = CommandSuSet::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_CREATOR)
            return CommandSuSet::commands();
        else
            return QStringList();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_CREATOR)
        {
            CommandSuSet::sendUsage(player, true);
            return true;
        }
        else
        {
            return false;
        }
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(args)

        if (!player || player->level() >= Player::PlayerLevel::PLAYER_LEVEL_CREATOR)
            return Command::Ptr(new CommandSuSet(args[0].value<CommandSuSet::ActionType>()));
        else
            return Command::Ptr(new CommandNull());
    }
};

} // namespace CORE

#endif // COMMANDSUSET_H
