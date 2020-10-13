/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSUINFO_H
#define COMMANDSUINFO_H

#include "command.h"
#include "item.h"

namespace CORE
{

class Map;

class CommandSuInfo : public Command
{
    friend class Command;
    friend class CommandSuInfoHandler;

public:
    CommandSuInfo();
    virtual ~CommandSuInfo();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    void sendChList(Player *player, QString filter = "", QMUD::IdType firstId = 0);
    void sendChInfo(Character *ch, Player *player);
    void sendItemList(Player *player, QString filter = "", QMUD::IdType firstId = 0);
    void sendItemInfo(Item *item, Player *player);
    void sendItemBaseBonusInfo(const QMUD::ItemBaseBonus& baseBonus, Player *player);
    void sendItemBaseTemporaryStatusInfo(const QMUD::ItemBaseStatus& baseStatus, Player *player);
    void sendMapInfo(Map* map, Player *player);
    void sendRoomInfo(Room* room, Player *player);
};

} // namespace CORE

namespace CORE
{

class CommandSuInfoHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandSuInfo::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_ADMIN)
            return CommandSuInfo::commands();
        else
            return QStringList();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        if (player->level() >= Player::PlayerLevel::PLAYER_LEVEL_ADMIN)
        {
            CommandSuInfo::sendUsage(player, true);
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
            return Command::Ptr(new CommandSuInfo());
        else
            return Command::Ptr(new CommandNull());
    }
};

} // namespace CORE

#endif // COMMANDSUINFO_H
