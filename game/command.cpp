/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "command.h"
#include "commandempty.h"
#include "commandclose.h"
#include "commandlook.h"
#include "commandmove.h"
#include "commandwho.h"
#include "commandcondition.h"
#include "commandkill.h"
#include "commandtake.h"
#include "commandinventory.h"
#include "commandwear.h"
#include "commandtakeoff.h"
#include "commandequipment.h"
#include "commanddrop.h"
#include "commandability.h"
#include "commandgroup.h"
#include "commandfollow.h"
#include "commandspell.h"
#include "commandsay.h"
#include "commandinstance.h"
#include "commandloot.h"
#include "commandshop.h"
#include "commandresources.h"
#include "commandeat.h"
#include "commandcrafting.h"
#include "commandaction.h"
#include "commandstatistics.h"
#include "commanddate.h"
#include "commandinfo.h"
#include "commandtitle.h"
#include "commandquest.h"
#include "commandtarget.h"

#include "commandsukill.h"
#include "commandsureloadmap.h"
#include "commandsuinfo.h"
#include "commandsuset.h"
#include "commandsugoto.h"
#include "commandsuban.h"

#include "room.h"
#include "item.h"
#include "world.h"
#include "../globaldebug.h"

#include <QObject>

using namespace CORE;

Command::Command() :
    m_ptrCh(Q_NULLPTR)
{

}

bool Command::isOf(QString cmd, QString str)
{
    return QString(cmd).startsWith(str, Qt::CaseInsensitive);
}

QString Command::command() const
{
    return m_strCommand;
}

QStringList Command::args() const
{
    return m_strArgs;
}

Entity *Command::findTargetInRoom(QString str, bool& wrongUsage, bool visibleOnly)
{
    Q_ASSERT(m_ptrCh);

    QStringList params = str.split(".");
    QString name;
    int index = 0;
    wrongUsage = true;

    if (params.isEmpty() || params.size() > 2)
        return Q_NULLPTR;

    if (params.size() == 1)
        name = params[0];
    else
    {
        name = params[1];
        index = params[0].toInt();
    }

    if (index < 0)
        return Q_NULLPTR;

    wrongUsage = false;

    Room* room = m_ptrCh->room();

    Q_ASSERT(room);

    QList<Entity*> entities = room->entities(visibleOnly ? m_ptrCh : Q_NULLPTR);

    for (auto it = entities.begin(); it != entities.end(); ++it)
    {
        if ((*it)->name(Q_NULLPTR).contains(name, Qt::CaseInsensitive))
        {
            index--;

            if (index <= 0)
                return *it;
        }
    }

    return Q_NULLPTR;
}

Character *Command::findCharacterInMap(QString str, bool& wrongUsage, bool visibleOnly)
{
    Q_ASSERT(m_ptrCh);

    QStringList params = str.split(".");
    QString name;
    int index = 0;
    wrongUsage = true;

    if (params.isEmpty() || params.size() > 2)
        return Q_NULLPTR;

    if (params.size() == 1)
        name = params[0];
    else
    {
        name = params[1];
        index = params[0].toInt();
    }

    if (index < 0)
        return Q_NULLPTR;

    wrongUsage = false;

    Map* map = m_ptrCh->room()->map();

    Q_ASSERT(map);

    auto characters = map->characters();

    for (auto it = characters.begin(); it != characters.end(); ++it)
    {
        if (visibleOnly && !m_ptrCh->canSee(*it))
            continue;

        if ((*it)->name(Q_NULLPTR).contains(name, Qt::CaseInsensitive))
        {
            index--;

            if (index <= 0)
                return *it;
        }
    }

    return Q_NULLPTR;
}

Item *Command::findItemInInventory(QString str, QVector<Item*> inventory, bool &wrongUsage)
{
    Q_ASSERT(m_ptrCh);

    QStringList params = str.split(".");
    QString name;
    int index = 0;
    wrongUsage = true;

    if (params.isEmpty() || params.size() > 2)
        return Q_NULLPTR;

    if (params.size() == 1)
        name = params[0];
    else
    {
        name = params[1];
        index = params[0].toInt();
    }

    if (index < 0)
        return Q_NULLPTR;

    wrongUsage = false;

    for (auto it = inventory.begin(); it != inventory.end(); ++it)
    {
        if ((*it)->name().contains(name, Qt::CaseInsensitive))
        {
            index--;

            if (index <= 0)
                return *it;
        }
    }

    return Q_NULLPTR;
}

Character *Command::findCharacterInRoom(QMUD::IdType chId, bool visibleOnly)
{
    Q_ASSERT(m_ptrCh);

    Room* room = qobject_cast<Room*>(m_ptrCh->parent());

    Q_ASSERT(room);

    QList<Entity*> entities = room->entities(visibleOnly ? m_ptrCh : Q_NULLPTR);

    for (auto it = entities.begin(); it != entities.end(); ++it)
    {
        if ((*it)->entityType() == Entity::EntityType::CHARACTER)
        {
            Character *chTmp = static_cast<Character*>(*it);

            if (chTmp->id() == chId)
                return chTmp;
        }
    }

    return Q_NULLPTR;
}

void Command::init(QString command, QStringList args, Character *ch)
{
    m_strCommand = command;
    m_strArgs = args;
    m_ptrCh = ch;
}

Command::~Command()
{

}

Command::Ptr Command::parse(QString str, Character *ch)
{
    static QVector<CommandHandler::Ptr> handlers;

    if (handlers.isEmpty())
    {
        handlers.push_back(CommandHandler::Ptr(new CommandEmptyHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandConditionHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandLookHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandInventoryHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandCloseHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandWhoHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandEatHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandCraftingHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandMoveHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandLootHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandResourcesHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandShopHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandAbilityHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandInstanceHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandTakeOffHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandKillHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandTakeHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandWearHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandFollowHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandEquipmentHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandDropHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandSpellHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandSayHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandGroupHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandActionHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandStatisticsHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandDateHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandInfoHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandTitleHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandQuestHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandTargetHandler()));

        // Comandi admin
        handlers.push_back(CommandHandler::Ptr(new CommandSuInfoHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandSuKillHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandSuSetHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandSuGotoHandler()));
        handlers.push_back(CommandHandler::Ptr(new CommandSuBanHandler()));

        // Comandi creatori
        handlers.push_back(CommandHandler::Ptr(new CommandSuReloadMapHandler()));
    }

    Command::Ptr cmd;
    QStringList args;

    if (!splitCommandArgs(args,  str.simplified()))
    {
        cmd = Command::Ptr(new CommandNull());
        cmd->init("", args.mid(1), ch);
    }
    else
    {
        if (args.isEmpty())
        {
            cmd = Command::Ptr(new CommandEmpty());
            cmd->init("", args.mid(1), ch);
        }
        else if (ch->chType() == Character::ChType::CHTYPE_PC &&
                 args[0].simplified().compare(QObject::tr("aiuto"), Qt::CaseInsensitive) == 0)
        {
            PcCharacter *pc = static_cast<PcCharacter*>(ch);

            if (args.size() == 1)
            {
                QStringList strList;

                for (auto handler : handlers)
                    strList.append(handler->commands(pc->player()));

                strList.sort(Qt::CaseInsensitive);

                pc->player()->sendLine(QObject::tr("Elenco dei comandi disponibili:"));

                int count = 0;
                for (auto cmd : strList)
                {
                    pc->player()->send(cmd.leftJustified(20));

                    if (++count % 4 == 0)
                        pc->player()->sendLine("");
                }

                if (count % 4 != 0)
                    pc->player()->sendLine("");

                pc->player()->sendLine("");
                pc->player()->sendLine(QObject::tr("Usa 'aiuto <comando>' per un aiuto specifico"));
            }
            else
            {
                QVector<QVariant> cmdVArgs;
                QString strCmd = args[1].simplified().toLower();
                bool cmdSended = false;

                for (auto handler : handlers)
                {
                    if (handler->isOf(strCmd, cmdVArgs))
                    {
                        cmdSended = handler->sendUsage(pc->player(), args.mid(1));
                        break;
                    }
                }

                if (!cmdSended)
                    pc->player()->sendLine(QObject::tr("Comando '%1' non riconosciuto").arg(args[1].simplified()));
            }

            cmd = Command::Ptr(new CommandEmpty());
            cmd->init("", args.mid(1), ch);
        }
        else
        {
            QString strCmd = args[0].simplified().toLower();

            for (auto handler : handlers)
            {
                QVector<QVariant> cmdVArgs;

                if (handler->isOf(strCmd, cmdVArgs))
                {
                    cmdVArgs.push_back(str);

                    if (ch->chType() == Character::ChType::CHTYPE_PC)
                        cmd = handler->create(static_cast<PcCharacter*>(ch)->player(), cmdVArgs);
                    else
                        cmd = handler->create(Q_NULLPTR, cmdVArgs);

                    break;
                }
            }

            if (!cmd)
                cmd = Command::Ptr(new CommandNull());

            cmd->init(strCmd, args.mid(1), ch);
        }
    }

    return cmd;
}

void Command::execute(bool confirmation)
{
    if (m_ptrCh->chType() == Character::ChType::CHTYPE_PC)
    {
        Player *player = static_cast<PcCharacter*>(m_ptrCh)->player();

        if (!confirmation)
            executePc(m_ptrCh, player);
        else
            executePcConfirmation(m_ptrCh, player);
    }
    else if (m_ptrCh->chType() == Character::ChType::CHTYPE_NPC)
        executeNpc(m_ptrCh);
    else
        GLOBAL_CRITICAL(QObject::tr("Tipo di character non gestito: %1").arg(static_cast<int>(m_ptrCh->chType())));

    QMUD::qmudWorld->flushPlayers();
}

void CommandNull::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    player->sendLine(QMUD_GLOBAL_STR_COMMAND_NOT_VALID);
}

void CommandNull::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

bool CORE::Command::splitCommandArgs(QStringList &qargs, QString command, int maxArgs)
{
    int len = command.length();
    bool qot = false;
    bool sqot = false;
    int arglen;

    for (int i = 0; i < len; i++)
    {
        int start = i;

        if (command[i] == '\"')
            qot = true;
        else if(command[i] == '\'')
            sqot = true;

        if (qot)
        {
            i++;
            start++;

            while (i < len && command[i] != '\"')
                i++;

            if (i < len)
                qot = false;

            arglen = i - start;
            i++;
        }
        else if (sqot)
        {
            i++;
            start++;

            while (i < len && command[i] != '\'')
                i++;

            if (i < len)
                sqot = false;

            arglen = i - start;
            i++;
        }
        else
        {
            while (i<len && !command[i].isSpace())
                i++;
            arglen = i - start;
        }

        if (maxArgs < 0 || maxArgs > qargs.size())
            qargs.push_back(command.mid(start, arglen));
        else
        {
            qargs.push_back(command.mid(start));
            break;
        }
    }

    return !(qot || sqot);
}
