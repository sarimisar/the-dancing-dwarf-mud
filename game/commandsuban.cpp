/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandsuban.h"
#include "world.h"
#include "room.h"

using namespace CORE;

CommandSuBan::CommandSuBan(ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandSuBan::~CommandSuBan()
{

}

bool CommandSuBan::isOf(QString cmd, ActionType &action)
{
    if (Command::isOf(QObject::tr("suban"), cmd) ||
            Command::isOf(QObject::tr("suban"), cmd))
    {
        action = ActionType::BAN;
        return true;
    }

    return false;
}

QStringList CommandSuBan::commands()
{
    return QStringList() << QObject::tr("suban");
}

void CommandSuBan::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    if (m_eAction == ActionType::BAN)
    {
        if (args().size() != 2 && args().size() != 3)
        {
            sendUsage(player);
            return;
        }

        if (QString("name").startsWith(args()[0]))
        {
            if (QString("add").startsWith(args()[1]) && args().size() == 3)
            {
                if (QMUD::qmudWorld->addForbiddenName(args()[2]))
                    player->sendLine(QObject::tr("Aggiunto %1 alla lista dei nomi bannati").arg(args()[2]));
                else
                    player->sendLine(QObject::tr("Il nome %1 e' gia' presente nella lista").arg(args()[2]));

                return;
            }
            else if (QString("remove").startsWith(args()[1]) && args().size() == 3)
            {
                if (QMUD::qmudWorld->removeForbiddenName(args()[2]))
                    player->sendLine(QObject::tr("Rimosso %1 alla lista dei nomi bannati").arg(args()[2]));
                else
                    player->sendLine(QObject::tr("Il nome %1 non e' presente nella lista").arg(args()[2]));

                return;
            }
            else if (QString("show").startsWith(args()[1]))
            {
                auto list = QMUD::qmudWorld->forbiddenPcNames();

                QString filter = args().size() == 3 ? args()[2].simplified().toLower() : "";

                bool printSomething = false;

                for (auto name : list)
                {
                    if (name.contains(filter))
                    {
                        printSomething = true;
                        player->sendLine(name);
                    }
                }

                if (!printSomething)
                {
                    if (filter.isEmpty())
                        player->sendLine(QObject::tr("La lista non contiene nessun nome"));
                    else
                        player->sendLine(QObject::tr("La lista non contiene nessun nome che rispetti il filtro %1").arg(args()[2]));
                }

                return;
            }
        }
    }

    sendUsage(player);
    return;
}

void CommandSuBan::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandSuBan::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: suban name add <nome> - Aggiunge <nome> alla lista ban"
                                 "     suban name remove <nome> - Rimuove <nome> dalla lista ban"
                                 "     suban name show [filtro] - Mostra la lista dei nomi bannati che contengono [filtro]"));
}
