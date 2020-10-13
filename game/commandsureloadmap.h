/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSURELOADAREA_H
#define COMMANDSURELOADAREA_H

#include "command.h"

namespace CORE
{

class CommandSuReloadMap : public Command
{
    friend class Command;
    friend class CommandSuReloadMapHandler;

public:
    CommandSuReloadMap();
    virtual ~CommandSuReloadMap();

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

class CommandSuReloadMapHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandSuReloadMap::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_ADMIN)
            return CommandSuReloadMap::commands();
        else
            return QStringList();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_CREATOR)
        {
            CommandSuReloadMap::sendUsage(player, true);
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
            return Command::Ptr(new CommandSuReloadMap());
        else
            return Command::Ptr(new CommandNull());
    }
};

} // namespace CORE

#endif // COMMANDSURELOADAREA_H
