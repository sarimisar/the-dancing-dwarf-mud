/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandsureloadmap.h"
#include "world.h"
#include "room.h"

using namespace CORE;

CommandSuReloadMap::CommandSuReloadMap() :
    Command()
{

}

CommandSuReloadMap::~CommandSuReloadMap()
{

}

bool CommandSuReloadMap::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("suricaricaarea"), cmd);
}

QStringList CommandSuReloadMap::commands()
{
    return QStringList() << QObject::tr("suricaricaarea");
}

void CommandSuReloadMap::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    if (args().size() != 1)
    {
        sendUsage(player);
        return;
    }

    bool ok;
    int mapId = args()[0].toInt(&ok);

    if (!ok)
    {
        sendUsage(player);
        return;
    }

    //if (QMUD::qmudWorld->reloadMap(static_cast<QMUD::StaticIdType>(mapId)))
    //    player->sendLine(QObject::tr("&YReload della mappa %1 completato con successo&d").arg(mapId));
    //else
        player->sendLine(QObject::tr("&RErrore durante il reload della mappa %1&d").arg(mapId));
}

void CommandSuReloadMap::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandSuReloadMap::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: suricaricaarea <id>"));
}
