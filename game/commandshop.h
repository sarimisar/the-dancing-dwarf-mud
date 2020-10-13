/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSHOP_H
#define COMMANDSHOP_H

#include "command.h"

namespace CORE
{

class CommandShop : public Command
{
    friend class Command;
    friend class CommandShopHandler;

public:
    enum class ActionType
    {
        LIST,
        BUY
    };

    CommandShop(ActionType action);
    virtual ~CommandShop();

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

Q_DECLARE_METATYPE(CORE::CommandShop::ActionType)

namespace CORE
{

class CommandShopHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandShop::ActionType action;
        bool ret = CommandShop::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandShop::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandShop::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandShop(args[0].value<CommandShop::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDSHOP_H
