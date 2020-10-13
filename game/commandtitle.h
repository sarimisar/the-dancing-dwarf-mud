/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDTITLE_H
#define COMMANDTITLE_H

#include "command.h"

namespace CORE
{

class CommandTitle : public Command
{
    friend class Command;
    friend class CommandTitleHandler;

public:
    CommandTitle(QString origCmd);
    virtual ~CommandTitle();

protected:
    static bool isOf(QString cmd);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character* ch, Player* player);
    void executeNpc(Character* ch);

private:
    QString m_strOrigCmd;
};

} // namespace CORE

namespace CORE
{

class CommandTitleHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        Q_UNUSED(args)

        return CommandTitle::isOf(cmd);
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandTitle::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandTitle::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)
        Q_UNUSED(args)

        return Command::Ptr(new CommandTitle(args[0].toString()));
    }
};

} // namespace CORE

#endif // COMMANDTITLE_H
