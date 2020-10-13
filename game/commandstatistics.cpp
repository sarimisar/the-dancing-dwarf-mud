/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandstatistics.h"
#include "world.h"
#include "characterclass.h"

using namespace CORE;

CommandStatistics::CommandStatistics() :
    Command()
{

}

CommandStatistics::~CommandStatistics()
{

}

bool CommandStatistics::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("statistiche"), cmd);
}

QStringList CommandStatistics::commands()
{
    return QStringList() << QObject::tr("statistiche");
}

void CommandStatistics::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    // Info generali e statistiche
    GlobalStatistics stat = QMUD::qmudWorld->statistics();
    stat += QMUD::qmudWorld->statisticsFromLastReboot();
    GlobalStatistics statLB = QMUD::qmudWorld->statisticsFromLastReboot();

    player->sendLine("&Y--------------------------------------------------------&d");

    player->sendLine(QObject::tr("&YUccisioni (NPC)  &d: %1 (LB: %2)").arg(stat.numberOfKilledNPC()).arg(statLB.numberOfKilledNPC()));
    player->sendLine(QObject::tr("&YUccisioni (PC)   &d: %1 (LB: %2)").arg(stat.numberOfKilledPC()).arg(statLB.numberOfKilledPC()));
    player->sendLine(QObject::tr("&YConnessioni      &d: %1 (LB: %2)").arg(stat.numberOfConnections()).arg(statLB.numberOfConnections()));

    auto resources = stat.resources();
    auto resourcesLB = statLB.resources();
    auto spentResources = stat.spentResources();
    auto spentResourcesLB = statLB.spentResources();

    if (!resources.isEmpty() || !resourcesLB.isEmpty() || !spentResources.isEmpty() || !spentResourcesLB.isEmpty())
    {
        player->sendLine(QObject::tr("&Y-------------- RISORSE (guadagnate/spese) --------------&d"));

        for (int i = static_cast<int>(QMUD::ResourceType::RESOURCE_FIRST); i < static_cast<int>(QMUD::ResourceType::MAX_VALUE); ++i)
        {
            auto itRes = resources.find(static_cast<QMUD::ResourceType>(i));
            auto itResLB = resourcesLB.find(static_cast<QMUD::ResourceType>(i));
            auto itSpentRes = spentResources.find(static_cast<QMUD::ResourceType>(i));
            auto itSpentResLB = spentResourcesLB.find(static_cast<QMUD::ResourceType>(i));

            if (itRes == resources.end() && itResLB == resourcesLB.end() && itSpentRes == spentResources.end() && itSpentResLB == spentResourcesLB.end())
                continue;

            quint64 res = (itRes == resources.end()) ? 0 : itRes.value();
            quint64 resLB = (itResLB == resourcesLB.end()) ? 0 : itResLB.value();
            quint64 spentRes = (itSpentRes == spentResources.end()) ? 0 : itSpentRes.value();
            quint64 spentResLB = (itSpentResLB == spentResourcesLB.end()) ? 0 : itSpentResLB.value();

            player->sendLine(QObject::tr("&Y%1 &d: %2 / %4 (LB: %3 / %5)")
                             .arg(QMUD::capitalize(QMUD::resourceToReadableString(itRes.key())).leftJustified(16))
                             .arg(res)
                             .arg(resLB)
                             .arg(spentRes)
                             .arg(spentResLB));
        }
    }

    player->sendLine("&Y--------------------------------------------------------&d");
}

void CommandStatistics::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandStatistics::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Mostra l'elenco delle statistiche globali relativi al MUD"));
        }

        player->sendLine(QObject::tr("Uso: statistiche [mondo]"));
    }
}
