/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandsugoto.h"
#include "world.h"
#include "room.h"
#include "item.h"
#include "itemarmor.h"
#include "itemweapon.h"
#include "itemtrinket.h"
#include "itemfood.h"
#include "itemdrink.h"
#include "itempotion.h"
#include "itemresource.h"
#include "itemtonic.h"
#include "npccharacter.h"
#include "pccharacter.h"

#include "../global.h"

using namespace CORE;

CommandSuGoto::CommandSuGoto() :
    Command()
{

}

CommandSuGoto::~CommandSuGoto()
{

}

bool CommandSuGoto::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("sugoto"), cmd);
}

QStringList CommandSuGoto::commands()
{
    return QStringList() << QObject::tr("sugoto");
}

void CommandSuGoto::executePc(Character *ch, Player* player)
{
    if (args().size() != 1)
    {
        sendUsage(player);
        return;
    }

    // Gestione GOTO MAP_ID:ROOM_ID
    QStringList roomArgs = args()[0].split(":");

    if (roomArgs.size() == 2)
    {
        QRegExp re("\\d*");

        if (re.exactMatch(roomArgs[0]) && re.exactMatch(roomArgs[1]))
        {
            QMUD::StaticIdType mapId = roomArgs[0].toUInt();
            QMUD::StaticIdType roomId = roomArgs[1].toUInt();

            auto map = QMUD::qmudWorld->map(mapId);

            if (map)
            {
                auto room = map->room(roomId);

                if (room)
                {
                    ch->setParent(room);
                    player->pcCharacter()->sendLook();
                    room->sendLine(ch, QObject::tr("%1 appare dal nulla").arg(ch->name(Q_NULLPTR)));
                    return;
                }
            }
        }
    }

    // Gestione GOTO NAME
    auto characters = QMUD::qmudWorld->characters();

    for (auto c : characters)
    {
        if (c->name(Q_NULLPTR).contains(args()[0], Qt::CaseInsensitive))
        {
            ch->setParent(c->room());
            player->pcCharacter()->sendLook();
            c->room()->sendLine(ch, QObject::tr("%1 appare dal nulla").arg(ch->name(Q_NULLPTR)));
            return;
        }
    }

    player->sendLine(QObject::tr("Nessun %1 trovato").arg(args()[0]));
}

void CommandSuGoto::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandSuGoto::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: sugoto <map_id>:<room_id>" QMUD_GLOBAL_NEW_LINE
                                 "Uso: sugoto <nome>"));
}
