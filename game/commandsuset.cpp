/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandsuset.h"
#include "world.h"
#include "room.h"

using namespace CORE;

CommandSuSet::CommandSuSet(ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandSuSet::~CommandSuSet()
{

}

bool CommandSuSet::isOf(QString cmd, ActionType &action)
{
    if (Command::isOf(QObject::tr("sulivello"), cmd) ||
            Command::isOf(QObject::tr("sulivella"), cmd))
    {
        action = ActionType::LEVELUP;
        return true;
    }
    else if (Command::isOf(QObject::tr("suset"), cmd))
    {
        action = ActionType::SET;
        return true;
    }

    return false;
}

QStringList CommandSuSet::commands()
{
    return QStringList() << QObject::tr("sulivello") << QObject::tr("suplayerset");
}

void CommandSuSet::executePc(Character *ch, Player* player)
{
    if (m_eAction == ActionType::LEVELUP)
    {
        if (args().size() != 1 && args().size() != 2)
        {
            sendUsage(player);
            return;
        }

        bool wrongUsage = false;
        Entity* targetEntity = findTargetInRoom(args()[0], wrongUsage, true);

        if (wrongUsage)
        {
            if (player)
                sendUsage(player);
            return;
        }

        if (!targetEntity)
        {
            if (args()[0].toLower() == QObject::tr("me"))
                targetEntity = ch;
            else
            {
                player->sendLine(QObject::tr("%1 non e' qui").arg(args()[0]));
                return;
            }
        }

        if (targetEntity->entityType() == Entity::EntityType::CHARACTER)
        {
            Character* targetCh = static_cast<Character*>(targetEntity);

            int numberOfLevels = 1;

            if (args().size() == 2)
                numberOfLevels = args()[1].toInt();

            if (numberOfLevels <= 0)
            {
                sendUsage(player);
                return;
            }

            int levelUp = 0;

            while (numberOfLevels > 0)
            {
                qint64 experienceToLevelUp = targetCh->chClass()->experienceToLevelUp(targetCh->level());

                if (experienceToLevelUp == 0)
                    break;

                targetCh->addExperience(experienceToLevelUp + 1);
                numberOfLevels--;
                levelUp++;
            }

            player->sendLine(QObject::tr("Hai donato %1 livelli a %2").arg(levelUp).arg(targetCh->name(Q_NULLPTR)));
        }
        else
        {
            player->sendLine(QObject::tr("%1 non e' un target valido").arg(args()[0]));
        }
    }
    else if (m_eAction == ActionType::SET)
    {
        if (args().size() < 2)
        {
            sendUsage(player);
            return;
        }

        bool wrongUsage = false;
        Entity* targetEntity = findTargetInRoom(args()[0], wrongUsage, true);

        if (wrongUsage)
        {
            if (player)
                sendUsage(player);
            return;
        }

        if (!targetEntity)
        {
            if (args()[0].toLower() == QObject::tr("me"))
                targetEntity = ch;
            else
            {
                player->sendLine(QObject::tr("%1 non e' qui").arg(args()[0]));
                return;
            }
        }

        if (targetEntity->entityType() == Entity::EntityType::CHARACTER)
        {
            Character* targetCh = static_cast<Character*>(targetEntity);

            // ----------------------------------------------------------
            // playerlevel <livello>
            // ----------------------------------------------------------
            if (QString("playerlevel").startsWith(args()[1]))
            {
                if (args().size() < 3)
                {
                    sendUsage(player);
                    return;
                }

                if (targetCh->chType() == Character::ChType::CHTYPE_PC)
                {
                    if (QString("normal").startsWith(args()[2]))
                    {
                        static_cast<PcCharacter*>(targetCh)->player()->setLevel(Player::PlayerLevel::PLAYER_LEVEL_NORMAL);
                        player->sendLine(QObject::tr("Hai impostato il player level di %1 a normale").arg(targetCh->name(Q_NULLPTR)));
                    }
                    else if (QString("admin").startsWith(args()[2]))
                    {
                        static_cast<PcCharacter*>(targetCh)->player()->setLevel(Player::PlayerLevel::PLAYER_LEVEL_ADMIN);
                        player->sendLine(QObject::tr("Hai impostato il player level di %1 a amministratore").arg(targetCh->name(Q_NULLPTR)));
                    }
                    else if (QString("creator").startsWith(args()[2]))
                    {
                        static_cast<PcCharacter*>(targetCh)->player()->setLevel(Player::PlayerLevel::PLAYER_LEVEL_CREATOR);
                        player->sendLine(QObject::tr("Hai impostato il player level di %1 a creatore").arg(targetCh->name(Q_NULLPTR)));
                    }
                    else
                    {
                        player->sendLine(QObject::tr("I valori validi di playerlevel sono normal, admin e creator"));
                    }
                }
                else
                    player->sendLine(QObject::tr("%1 non e' un target valido").arg(args()[0]));

                return;
            }
            // ----------------------------------------------------------
            // resource <risorsa> <add/remove> <quantita>
            // ----------------------------------------------------------
            else if (QString("resource").startsWith(args()[1]))
            {
                if (args().size() <= 2)
                {
                    showResourceList(player);
                    sendUsageSuSetResource(player);
                    return;
                }

                if (args().size() != 5)
                {
                    sendUsageSuSetResource(player);
                    return;
                }

                auto resource = QMUD::resourceFromString(args()[2]);

                if (resource == QMUD::ResourceType::UNKNOWN)
                {
                    showResourceList(player);
                    player->sendLine(QObject::tr("&R%1 non e' una risorsa valida&d").arg(args()[2]));
                    return;
                }

                bool addOrRemove = false;
                if (QString("add").startsWith(args()[3]))
                    addOrRemove = true;
                else if (QString("remove").startsWith(args()[3]))
                    addOrRemove = false;
                else
                {
                    player->sendLine(QObject::tr("&R%1 non e' un parametro valido&d").arg(args()[3]));
                    sendUsageSuSetResource(player);
                    return;
                }

                QRegExp re("\\d*");
                if (!re.exactMatch(args()[4]))
                {
                    player->sendLine(QObject::tr("&R%1 non e' un parametro valido&d").arg(args()[4]));
                    sendUsageSuSetResource(player);
                    return;
                }

                int quantity = qMax(args()[4].toInt(), 0);

                if (addOrRemove)
                {
                    player->sendLine(QObject::tr("Aggiungi %1 %2 alle scorte di %3")
                                     .arg(quantity)
                                     .arg(QMUD::resourceToReadableString(resource))
                                     .arg(targetCh->name(Q_NULLPTR)));

                    targetCh->addResource(resource, quantity, false);
                }
                else
                {
                    player->sendLine(QObject::tr("Rimuovi %1 %2 dalle scorte di %3")
                                     .arg(quantity)
                                     .arg(QMUD::resourceToReadableString(resource))
                                     .arg(targetCh->name(Q_NULLPTR)));


                    targetCh->removeResource(resource, quantity);
                }

                return;
            }
            else
            {
                sendUsage(player);
                return;
            }
        }
        else
        {
            player->sendLine(QObject::tr("%1 non e' un target valido").arg(args()[0]));
        }
    }
    else
    {
        sendUsage(player);
        return;
    }
}

void CommandSuSet::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandSuSet::showResourceList(Player *player)
{
    player->sendLine(QObject::tr("Elenco delle risorse disponibili:"));

    for (int i = static_cast<int>(QMUD::ResourceType::RESOURCE_FIRST); i < static_cast<int>(QMUD::ResourceType::MAX_VALUE); ++i)
    {
        player->sendLine(QObject::tr(" &W%1&d - [%2] %3")
                         .arg(QMUD::resourceToString(static_cast<QMUD::ResourceType>(i)), 26)
                         .arg(QMUD::resourceToLevel(static_cast<QMUD::ResourceType>(i)))
                         .arg(QMUD::resourceToReadableString(static_cast<QMUD::ResourceType>(i))));

    }
}

void CommandSuSet::sendUsageSuSetResource(Player *player)
{
    player->sendLine(QObject::tr("Uso: suset [<n>.]<nome> resource <risorsa> <add/remove> <quantita>"));
}

void CommandSuSet::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: sulivello [<n>.]<nome> [numero di livelli]" QMUD_GLOBAL_NEW_LINE
                                 "        dona livelli da un character" QMUD_GLOBAL_NEW_LINE
                                 "     suset [<n>.]<nome> <opzione> [parametri]" QMUD_GLOBAL_NEW_LINE
                                 "        setta un parametro di un character" QMUD_GLOBAL_NEW_LINE
                                 "        opzioni:" QMUD_GLOBAL_NEW_LINE
                                 "          playerlevel <normal/admin/creator>" QMUD_GLOBAL_NEW_LINE
                                 "          resource    <risorsa> <add/remove> <quantita>"));
}
