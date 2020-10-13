/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDCRAFTING_H
#define COMMANDCRAFTING_H

#include "command.h"

namespace CORE
{

class CommandCrafting : public Command
{
    friend class Command;
    friend class CommandCraftingHandler;

public:
    enum class ActionType
    {
        RECIPES_LIST,
        PROFESSIONS_LIST,
        CRAFTING
    };

    CommandCrafting(ActionType action);
    virtual ~CommandCrafting();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    ActionType m_eAction;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandCrafting::ActionType)

namespace CORE
{

class CommandCraftingHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandCrafting::ActionType action;
        bool ret = CommandCrafting::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandCrafting::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandCrafting::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandCrafting(args[0].value<CommandCrafting::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDCRAFTING_H
