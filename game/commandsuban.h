/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSUBAN_H
#define COMMANDSUBAN_H

#include "command.h"

namespace CORE
{

class CommandSuBan : public Command
{
    friend class Command;
    friend class CommandSuBanHandler;

public:
    enum class ActionType
    {
        BAN
    };

    CommandSuBan(ActionType action);
    virtual ~CommandSuBan();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    ActionType m_eAction;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandSuBan::ActionType)

namespace CORE
{

class CommandSuBanHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandSuBan::ActionType action;
        bool ret = CommandSuBan::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_CREATOR)
            return CommandSuBan::commands();
        else
            return QStringList();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_CREATOR)
        {
            CommandSuBan::sendUsage(player, true);
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
            return Command::Ptr(new CommandSuBan(args[0].value<CommandSuBan::ActionType>()));
        else
            return Command::Ptr(new CommandNull());
    }
};

} // namespace CORE

#endif // COMMANDSUBAN_H
