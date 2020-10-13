/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandresources.h"
#include "world.h"
#include "characterclass.h"
#include "item.h"
#include "room.h"
#include "itemresource.h"

using namespace CORE;

CommandResources::CommandResources(ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandResources::~CommandResources()
{

}

bool CommandResources::isOf(QString cmd, ActionType& action)
{
    if (Command::isOf(QObject::tr("risorse"), cmd))
    {
        action = ActionType::LIST;
        return true;
    }
    else if (Command::isOf(QObject::tr("rradar"), cmd))
    {
        action = ActionType::RADAR;
        return true;
    }
    //else if (Command::isOf(QObject::tr("mina"), cmd) ||
    //         Command::isOf(QObject::tr("mino"), cmd))
    //{
    //    action = ActionType::MINING;
    //    return true;
    //}
    else if (Command::isOf(QObject::tr("raccogli"), cmd) ||
             Command::isOf(QObject::tr("raccolgo"), cmd))
    {
        action = ActionType::ERBALISM;
        return true;
    }

    return false;
}

QStringList CommandResources::commands()
{
    return QStringList() << QObject::tr("risorse")
                         << QObject::tr("rradar")
                         << QObject::tr("mino")
                         << QObject::tr("raccolgo");
}

void CommandResources::executePc(Character *ch, Player* player)
{
    if (m_eAction == ActionType::LIST)
    {
        player->sendLine("&Y--------------------------------------------------------&d");
        player->sendLine(QObject::tr("Elenco delle risorse disponibili:"));
        player->sendLine("");
        player->sendLine(QString("  &Y%1&d: %2")
                         .arg(QObject::tr("Aurei").leftJustified(20))
                         .arg(QMUD::coinsToReadableString(ch->coins())));

        auto resources = ch->resources();
        for (auto it = resources.begin(); it != resources.end(); ++it)
        {
            if (it.key() != QMUD::ResourceType::COINS)
            {
                QString strResource = QMUD::resourceToReadableString(it.key());

                player->sendLine(QString("  &Y%1&d: %2")
                                 .arg(QMUD::capitalize(strResource).leftJustified(20))
                                 .arg(it.value()));
            }
        }

        player->sendLine("&Y--------------------------------------------------------&d");
    }
    else if (m_eAction == ActionType::RADAR)
    {
        if (args().size() != 1)
        {
            sendUsage(player);
            return;
        }

        PcCharacter* pch = static_cast<PcCharacter*>(ch);

        if (QObject::tr("on").startsWith(args()[0], Qt::CaseInsensitive))
        {
            pch->setResourceRadarOn(true);
            player->sendLine(QObject::tr("Attivi il radar delle risorse"));
        }
        else if (QObject::tr("off").startsWith(args()[0], Qt::CaseInsensitive))
        {
            pch->setResourceRadarOn(false);
            player->sendLine(QObject::tr("Disattivi il radar delle risorse"));
        }
        else
        {
            sendUsage(player);
            return;
        }
    }
    else if (m_eAction == ActionType::MINING ||
             m_eAction == ActionType::ERBALISM)
    {
        if (args().size() != 1)
        {
            sendUsage(player);
            return;
        }

        if (ch->isUnconscious())
        {
            if (player)
                player->sendLine(QObject::tr("Sei incoscente"));

            return;
        }

        // Controllo se posso agire
        QString message;
        if (ch->canMakeAction(message))
        {
            if (player)
                player->sendLine(message);
        }

        bool wrongUsage = false;
        Entity* targetEntity = findTargetInRoom(args()[0], wrongUsage, true);

        if (wrongUsage)
        {
            sendUsage(player);
            return;
        }

        if (!targetEntity)
        {
            player->sendLine(QObject::tr("Non c'e' nessun '%1' qui").arg(args()[0]));
            return;
        }

        if (targetEntity->entityType() != Entity::EntityType::ITEM ||
            static_cast<Item*>(targetEntity)->itemType() != QMUD::ItemType::RESOURCE)
        {
            if (m_eAction == ActionType::MINING)
                player->sendLine(QObject::tr("Non puoi minare %1").arg(targetEntity->name(Q_NULLPTR)));
            else
                player->sendLine(QObject::tr("Non puoi raccogliere %1").arg(targetEntity->name(Q_NULLPTR)));
            return;
        }

        ItemResource* itemResource = static_cast<ItemResource*>(targetEntity);

        if ((m_eAction == ActionType::MINING && !QMUD::resourceIsForMining(itemResource->resource())) ||
            (m_eAction == ActionType::ERBALISM && !QMUD::resourceIsForErbalism(itemResource->resource())))
        {
            if (m_eAction == ActionType::MINING)
                player->sendLine(QObject::tr("Non puoi minare %1").arg(targetEntity->name(Q_NULLPTR)));
            else
                player->sendLine(QObject::tr("Non puoi raccogliere %1").arg(targetEntity->name(Q_NULLPTR)));
            return;
        }

        if (!itemResource->collectable())
        {
            if (m_eAction == ActionType::MINING)
                player->sendLine(QObject::tr("Non puoi estrarre altro da %1").arg(targetEntity->name(Q_NULLPTR)));
            else
                player->sendLine(QObject::tr("Non puoi raccogliere altro da %1").arg(targetEntity->name(Q_NULLPTR)));
            return;
        }

        int resources = itemResource->collect();

        ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 2, false);

        ch->addResource(itemResource->resource(), resources);
        player->sendLine(itemResource->action());

        auto characters = ch->room()->characters();

        for (Character* chInRoom : characters)
        {
            if (chInRoom != ch && chInRoom->chType() == Character::ChType::CHTYPE_PC)
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

                if (chInRoom->canSee(ch))
                {
                    playerOther->sendLine(itemResource->actionOther().arg(ch->name(Q_NULLPTR)));
                }
            }
        }

        player->sendLine(QObject::tr("&CGuadagni %1 %2&d").arg(resources).arg(QMUD::resourceToReadableString(itemResource->resource())));
    }
}

void CommandResources::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandResources::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Mostra l'elenco delle risorse disponibili"));
        }

        player->sendLine(QObject::tr("Uso: risorse          - mostra l'elenco delle risorse disponibili" QMUD_GLOBAL_NEW_LINE
                                     "     rradar <on/off>  - attiva o disattiva il radar risorse"));
    }
}
