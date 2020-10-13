/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandsukill.h"
#include "world.h"
#include "room.h"

using namespace CORE;

CommandSuKill::CommandSuKill() :
    Command()
{

}

CommandSuKill::~CommandSuKill()
{

}

bool CommandSuKill::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("suelimina"), cmd);
}

QStringList CommandSuKill::commands()
{
    return QStringList() << QObject::tr("suelimina");
}

void CommandSuKill::executePc(Character *ch, Player* player)
{
    if (args().size() != 1)
    {
        sendUsage(player);
        return;
    }

    bool wrongUsage = false;
    Entity* targetEntity = findTargetInRoom(args()[0], wrongUsage, false);

    if (wrongUsage)
    {
        sendUsage(player);
        return;
    }

    if (!targetEntity)
    {
        player->sendLine(QObject::tr("%1 non e' qui").arg(args()[0]));
        return;
    }

    if (targetEntity->entityType() == Entity::EntityType::CHARACTER)
    {
        Character *targetCh = dynamic_cast<Character*>(targetEntity);

        if (targetCh->chType() == Character::ChType::CHTYPE_PC)
        {
            dynamic_cast<PcCharacter*>(targetCh)->player()->sendLine(QObject::tr("%1 ti uccide con un solo sguardo! &WRIP&d").arg(ch->name(targetCh)));
        }

        auto characters = ch->room()->characters();

        QString msgOther = QObject::tr("%1 uccide %2 con un solo sguardo! &WRIP&d");

        for (Character* chInRoom : characters)
        {
            if (chInRoom != ch && chInRoom != targetCh && chInRoom->chType() == Character::ChType::CHTYPE_PC)
            {
                Player *player = static_cast<PcCharacter*>(chInRoom)->player();
                player->sendLine(msgOther.arg(ch->name(chInRoom)).arg(targetCh->name(chInRoom)));
            }
        }

        targetCh->kill();
    }
}

void CommandSuKill::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandSuKill::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: suelimina [<n>.]<nome>"));
}
