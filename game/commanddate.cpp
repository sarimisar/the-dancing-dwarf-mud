/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commanddate.h"
#include "world.h"
#include "characterclass.h"
#include "datetime.h"

using namespace CORE;

CommandDate::CommandDate() :
    Command()
{

}

CommandDate::~CommandDate()
{

}

bool CommandDate::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("data"), cmd) ||
           Command::isOf(QObject::tr("ora"), cmd);
}

QStringList CommandDate::commands()
{
    return QStringList() << QObject::tr("data") << QObject::tr("ora");
}

void CommandDate::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    player->sendLine(DateTime::currentDateTimeToString());
}

void CommandDate::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandDate::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Mostra la data e l'ora correnti"));
        }

        player->sendLine(QObject::tr("Uso: data/ora"));
    }
}
