/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDABILITY_H
#define COMMANDABILITY_H

#include "command.h"

namespace CORE
{

class CommandAbility : public Command
{
    friend class Command;
    friend class CommandAbilityHandler;

public:
    enum class ActionType
    {
        ABILITY,
        ABILITY_LIST
    };

    CommandAbility(QMUD::AbilityType abilityType, ActionType action);
    virtual ~CommandAbility();

protected:
    static bool isOf(QString cmd, QMUD::AbilityType &abilityType, ActionType &action);
    static QStringList commands();
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    void sendUsage(Player* player);
    QMUD::AbilityType m_eAbilityType;
    ActionType m_eActionType;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandAbility::ActionType)

namespace CORE
{

class CommandAbilityHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        QMUD::AbilityType ability;
        CommandAbility::ActionType action;
        bool ret = CommandAbility::isOf(cmd, ability, action);

        args.push_back(QVariant::fromValue(ability));
        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandAbility::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        if (args.isEmpty())
        {

            return false;
        }

        QMUD::AbilityType ability;
        CommandAbility::ActionType action;

        if (!CommandAbility::isOf(args[0], ability, action))
            return false;

        CommandAbility(ability, action).sendUsage(player);

        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandAbility(args[0].value<QMUD::AbilityType>(), args[1].value<CommandAbility::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDWEAR_H
