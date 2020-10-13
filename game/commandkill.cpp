/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandkill.h"
#include "world.h"

using namespace CORE;

CommandKill::CommandKill() :
    Command()
{

}

CommandKill::~CommandKill()
{

}

bool CommandKill::isOf(QString cmd)
{
    return Command::isOf("attack", cmd);
}

QStringList CommandKill::commands()
{
    return QStringList() << QObject::tr("attack");
}

void CommandKill::executePc(Character *ch, Player* player)
{
    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    if (ch->isInCombat())
        ch->setInCombat(false);
    else
    {
        auto targetCh = ch->targetCharacter(true);

        if (targetCh)
        {
            QString message;

            if (!ch->canAttack(targetCh, message))
            {
                if (player)
                    player->sendLine(message);

                return;
            }

            ch->setInCombat(true);

            if (ch->canMakeAction(message))
            {
                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 1, false);
                ch->executeAttack();
            }
        }
    }
}

void CommandKill::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandKill::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: uccido [<n>.]<nome>"));
}
