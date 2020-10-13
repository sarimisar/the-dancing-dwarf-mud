/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSAY_H
#define COMMANDSAY_H

#include "command.h"

namespace CORE
{

class CommandSay : public Command
{
    friend class Command;
    friend class CommandSayHandler;

public:
    CommandSay(QMUD::SayType sayType, QString origCmd);
    virtual ~CommandSay();

protected:
    static bool isOf(QString cmd, QMUD::SayType &sayType);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    QMUD::SayType m_eSayType;
    QString m_strOrigCmd;
};

} // namespace CORE

namespace CORE
{

class CommandSayHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        QMUD::SayType action;
        bool ret = CommandSay::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandSay::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandSay::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandSay(args[0].value<QMUD::SayType>(), args[1].toString()));
    }
};

} // namespace CORE

#endif // COMMANDSAY_H
