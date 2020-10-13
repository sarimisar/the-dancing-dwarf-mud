/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDINVENTORY_H
#define COMMANDINVENTORY_H

#include "command.h"

namespace CORE
{

class CommandInventory : public Command
{
    friend class Command;
    friend class CommandInventoryHandler;

public:
    enum class ActionType
    {
        INVENTORY,
        SACRIFY
    };

    CommandInventory(ActionType action);
    virtual ~CommandInventory();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executePcConfirmation(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    ActionType m_eAction;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandInventory::ActionType)

namespace CORE
{

class CommandInventoryHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandInventory::ActionType action;
        bool ret = CommandInventory::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandInventory::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandInventory::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandInventory(args[0].value<CommandInventory::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDINVENTORY_H
