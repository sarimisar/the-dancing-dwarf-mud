/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandwho.h"
#include "world.h"

using namespace CORE;

CommandWho::CommandWho() :
    Command()
{

}

CommandWho::~CommandWho()
{

}

bool CommandWho::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("chi"), cmd);
}

QStringList CommandWho::commands()
{
    return QStringList() << QObject::tr("chi");
}

void CommandWho::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    auto players = QMUD::qmudWorld->playersByConnectionId();
    int playersInGame = 0;

    for (auto it = players.begin(); it != players.end(); ++it)
    {
        PcCharacter *chP = it.value()->pcCharacter();

        if (chP && ch->canSee(chP))
        {
            player->sendLine(QString("   %1 - &W%2 &d[%3] %4&d")
                             .arg(chP->name(Q_NULLPTR), QMUD_GLOBAL_CH_NAME_MAX_SIZE)
                             .arg(chP->chClass()->toReadableString(), 9)
                             .arg(chP->level())
                             .arg(chP->title()));
            playersInGame++;
        }
    }

    player->sendLine(QString(QObject::tr("%1 giocatori connessi (%2 in gioco)")).arg(players.size()).arg(playersInGame));
}

void CommandWho::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandWho::sendUsage(Player *player, bool help)
{
    if (help)
    {
        player->sendLine(QObject::tr("Mostra l'elenco dei giocatori connessi"));
    }

    player->sendLine(QObject::tr("Uso: chi"));
}
