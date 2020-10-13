/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandtarget.h"
#include "world.h"
#include "room.h"
#include "map.h"

using namespace CORE;

CommandTarget::CommandTarget() :
    Command()
{

}

CommandTarget::~CommandTarget()
{

}

bool CommandTarget::isOf(QString cmd)
{
    return Command::isOf("target", cmd);
}

QStringList CommandTarget::commands()
{
    return QStringList() << "target";
}

void CommandTarget::executePc(Character *ch, Player* player)
{
    if (args().size() != 1)
    {
        if (player)
            sendUsage(player);
        return;
    }

    if (ch->room()->map()->character(args()[0].toULongLong()))
        ch->setTarget(args()[0].toULongLong(), true);

    if (player)
        player->pcCharacter()->sendTargetInfo();
}

void CommandTarget::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandTarget::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("target <id>"));
}
