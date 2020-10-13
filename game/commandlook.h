/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDLOOK_H
#define COMMANDLOOK_H

#include "command.h"

namespace CORE
{

class CommandLook : public Command
{
    friend class Command;
    friend class CommandLookHandler;

public:
    enum class ActionType
    {
        LOOK,
        LOOK_NO_PROMPT
    };

    CommandLook(ActionType type);
    virtual ~CommandLook();

protected:
    static bool isOf(QString cmd, ActionType &type);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);    

private:
    ActionType m_eActionType;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandLook::ActionType)

namespace CORE
{

class CommandLookHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandLook::ActionType action;
        bool ret = CommandLook::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandLook::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandLook::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandLook(args[0].value<CommandLook::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDLOOK_H
