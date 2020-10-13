/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDWEAR_H
#define COMMANDWEAR_H

#include "command.h"

namespace CORE
{

class CommandWear : public Command
{
    friend class Command;
    friend class CommandWearHandler;

public:
    enum class ActionType
    {
        HOLD,   // impugnare
        WEAR,   // indossare
        GRAB    // afferrare
    };

    CommandWear(ActionType action);
    virtual ~CommandWear();

protected:
    static bool isOf(QString cmd, ActionType& action);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    bool wearItem(Character* ch, Player *player, QList<Character *> characters, Item *targetItem);

private:
    ActionType m_eAction;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandWear::ActionType)

namespace CORE
{

class CommandWearHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandWear::ActionType action;
        bool ret = CommandWear::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandWear::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandWear::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandWear(args[0].value<CommandWear::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDWEAR_H
