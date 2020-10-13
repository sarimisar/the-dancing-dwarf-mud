/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDINFO_H
#define COMMANDINFO_H

#include "command.h"
#include "item.h"

namespace CORE
{

class Map;

class CommandInfo : public Command
{
    friend class Command;
    friend class CommandInfoHandler;

public:
    CommandInfo();
    virtual ~CommandInfo();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    void sendItemList(Player *player, QString filter = "", QMUD::IdType firstId = 0);
    void sendItemInfo(Item *item, Player *player);
    void sendItemBaseBonusInfo(const QMUD::ItemBaseBonus& baseBonus, Player *player);
    void sendItemBaseTemporaryStatusInfo(const QMUD::ItemBaseStatus& baseStatus, Player *player);
};

} // namespace CORE

namespace CORE
{

class CommandInfoHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandInfo::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandInfo::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandInfo::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandInfo());
    }
};

} // namespace CORE

#endif // COMMANDSUINFO_H
