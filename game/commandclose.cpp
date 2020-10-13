/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandclose.h"
#include "room.h"
#include "world.h"

using namespace CORE;

CommandClose::CommandClose() :
    Command()
{

}

CommandClose::~CommandClose()
{

}

bool CommandClose::isOf(QString cmd)
{
    return cmd == "esci";
}

QStringList CommandClose::commands()
{
    return QStringList() << QObject::tr("esci");
}

void CommandClose::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    if (ch->isInCombat())
    {
        player->sendLine(QObject::tr("Non puoi uscire dal gioco mentre stai combattendo!"));
        return;
    }

    // Controllo se posso agire
    QString message;
    if (ch->canMakeAction(message))
    {
        if (player)
            player->sendLine(message);
    }

    Room* room = ch->room();

    auto characters = room->characters();

    QString msgOther = QObject::tr("%1 esce dal gioco.").arg(ch->name(Q_NULLPTR));

    for (Character* chInRoom : characters)
    {
        if (chInRoom != ch && chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(ch))
        {
            Player *player = static_cast<PcCharacter*>(chInRoom)->player();
            player->sendLine(msgOther);
        }
    }

    player->sendLine(QObject::tr("&GTorna preso a trovarci!&d"));
    player->forceSave();
    player->connection()->close();
}

void CommandClose::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandClose::sendUsage(Player *player, bool help)
{
    Q_UNUSED(player)
    Q_UNUSED(help)
}
