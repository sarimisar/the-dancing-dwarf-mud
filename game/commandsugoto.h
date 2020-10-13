/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSUGOTO_H
#define COMMANDSUGOTO_H

#include "command.h"
#include "item.h"

namespace CORE
{

class Map;

class CommandSuGoto : public Command
{
    friend class Command;
    friend class CommandSuGotoHandler;

public:
    CommandSuGoto();
    virtual ~CommandSuGoto();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);
};

} // namespace CORE

namespace CORE
{

class CommandSuGotoHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandSuGoto::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_ADMIN)
            return CommandSuGoto::commands();
        else
            return QStringList();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_ADMIN)
        {
            CommandSuGoto::sendUsage(player, true);
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

        if (!player || player->level() >= Player::PlayerLevel::PLAYER_LEVEL_ADMIN)
            return Command::Ptr(new CommandSuGoto());
        else
            return Command::Ptr(new CommandNull());
    }
};

} // namespace CORE

#endif // COMMANDSUGOTO_H
