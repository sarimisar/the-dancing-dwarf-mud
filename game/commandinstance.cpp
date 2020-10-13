/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandinstance.h"
#include "world.h"
#include "room.h"
#include "group.h"

#include "../global.h"

using namespace CORE;

CommandInstance::CommandInstance() :
    Command()
{

}

CommandInstance::~CommandInstance()
{

}

bool CommandInstance::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("istanza"), cmd);
}

QStringList CommandInstance::commands()
{
    return QStringList() << QObject::tr("istanza");
}

void CommandInstance::executePc(Character *ch, Player* player)
{
    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    // ------------------------------------------
    // Lista delle istanze bind
    // ------------------------------------------
    if (args().size() == 0)
    {
        bool empty = true;

        for (auto instance : QMUD::qmudWorld->maps())
        {
            if (instance->isInstance() && instance->instancePcCharacterIsBind(ch->name(Q_NULLPTR)))
            {
                player->sendLine(QObject::tr("  %1: %2 - Ultima entrata %3 minuti fa")
                                 .arg(instance->staticId())
                                 .arg(instance->name())
                                 .arg(instance->instanceLastEntryTime().secsTo(QDateTime::currentDateTime())/60));
                empty = false;
            }
        }

        if (empty)
        {
            player->sendLine(QObject::tr("Non sei ancora entrato in nessuna istanza"));
            return;
        }
    }
    // ------------------------------------------
    // Reset di una istanza
    // ------------------------------------------
    else if (args().size() == 2)
    {
        QRegExp re("\\d*");

        if (args()[0] == "RESET" && re.exactMatch(args()[1]))
        {
            // Index dell'instanza
            unsigned int instanceId = args()[1].toUInt();

            for (auto instance : QMUD::qmudWorld->maps())
            {
                // Cerco l'istanza bindata al character e con lo static id specificato
                if (instance->isInstance() &&
                        instance->instancePcCharacterIsBind(ch->name(Q_NULLPTR)) &&
                        instanceId == instance->staticId())
                {
                    // Controllo se e' resettabile
                    if (instance->instanceCreationTime().addMSecs(QMUD_GLOBAL_TIMER_REMOVE_INACTIVE_INSTANCE_MS) > QDateTime::currentDateTime())
                    {
                        player->sendLine(QObject::tr("Devi attendere altri %1 minuti")
                                         .arg(QDateTime::currentDateTime()
                                              .msecsTo(instance->instanceCreationTime().addMSecs(QMUD_GLOBAL_TIMER_REMOVE_INACTIVE_INSTANCE_MS)) / 60000 + 1));
                        return;
                    }

                    // Controllo se ci sono giocatori all'interno dell'istanza.
                    // Mostro l'elenco al character
                    auto characters = instance->characters();
                    bool firstPcFound = true;

                    for (auto c : characters)
                    {
                        if (c->chType() == Character::ChType::CHTYPE_PC)
                        {
                            if (firstPcFound)
                                player->sendLine(QObject::tr("Ci sono ancora dei giocatori all'interno"));

                            player->sendLine(QString("  %1").arg(c->name(ch)));
                            firstPcFound = false;
                        }
                    }

                    if (!firstPcFound)
                        return;

                    // Rimuovo l'istanza dal world
                    player->sendLine(QObject::tr("Resetti l'istanza %1: %2").arg(instance->staticId()).arg(instance->name()));
                    QMUD::qmudWorld->removeMap(instance->staticId());

                    return;
                }
            }

            // Istanza non trovata
            player->sendLine(QObject::tr("Indice dell'istanza non valido"));
        }
        else
        {
            sendUsage(player);
        }
    }
    else if (args().size() == 1)
    {
        // ------------------------------------------
        // Entrare in una istanza
        // ------------------------------------------
        if (args()[0] == QObject::tr("entra"))
        {
            // Controllo se nella stanza c'e' un portale per una istanza
            Room *room = ch->room();

            if (room->instanceEntryPoint().isEmpty())
            {
                player->sendLine(QObject::tr("Non c'e' nessuna entrata ad una istanza qui"));
                return;
            }

            PcCharacter *pCh = static_cast<PcCharacter*>(ch);

            // Controllo se il character fa parte di un gruppo di cui
            // non e' leader. In tal caso entro nell'istanza solo se il leader
            // del gruppo si trova nella stessa istanza
            if (pCh->group() &&
                pCh->group()->leader() != pCh)
            {
                if (pCh->group()->leader()->room()->map()->isInstance())
                {
                    if (pCh->group()->leader()->room()->map()->instanceEntryPoint() == room->instanceEntryPoint())
                    {
                        // Controllo se esiste gia una istanza di proprieta'
                        // di questo PC
                        for (auto instance : QMUD::qmudWorld->maps())
                        {
                            if (instance->isInstance() &&
                                    instance->instanceEntryPoint() == room->instanceEntryPoint() &&
                                    instance->instancePcCharacterIsBind(ch->name(Q_NULLPTR)))
                            {
                                if (instance->originalStaticId() == pCh->group()->leader()->room()->map()->originalStaticId())
                                {
                                    player->sendLine(QObject::tr("Non puoi entrare nella stessa istanza del leader del tuo gruppo."));
                                    player->sendLine(QObject::tr("Sei gia' legato ad un'altra istanza di questa mappa"));
                                    return;
                                }
                            }
                        }

                        // Entro nell'istanza del leader del gruppo
                        auto chsFrom = ch->room()->characters();

                        for (auto chInRoom : chsFrom)
                            if (chInRoom != ch && chInRoom->canSee(ch) && chInRoom->chType() == Character::ChType::CHTYPE_PC)
                                static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 entra in un portale").arg(ch->name(Q_NULLPTR)));

                        player->send(QMUD_GLOBAL_NEW_LINE);
                        player->sendLine(QObject::tr("&WEntri nel portale e vieni teletrasportato all'entrata di '%1'&d").arg(pCh->group()->leader()->room()->map()->name()));
                        player->send(QMUD_GLOBAL_NEW_LINE);
                        ch->setParent(pCh->group()->leader()->room());
                        ch->room()->map()->instanceBindPcCharacter(ch->name(Q_NULLPTR));
                        static_cast<PcCharacter*>(ch)->sendLook();

                        auto chsTo = ch->room()->characters();

                        for (auto chInRoom : chsTo)
                            if (chInRoom != ch && chInRoom->canSee(ch) && chInRoom->chType() == Character::ChType::CHTYPE_PC)
                                static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 compare dal nulla").arg(ch->name(Q_NULLPTR)));
                    }
                    else
                    {
                        player->sendLine(QObject::tr("Il leader del tuo gruppo non e' all'interno di questa istanza"));
                    }
                }

                player->sendLine(QObject::tr("Devi essere il leader del tuo gruppo per entrare"));
            }
            else
            {
                Map* map = Q_NULLPTR;

                // Controllo se esiste gia una istanza di proprieta'
                // di questo PC
                for (auto instance : QMUD::qmudWorld->maps())
                {
                    if (instance->isInstance() &&
                            instance->instanceEntryPoint() == room->instanceEntryPoint() &&
                            instance->instancePcCharacterIsBind(ch->name(Q_NULLPTR)))
                        map = instance;
                }

                // Se non esiste alcuna istanza di proprieta' del PC allora genero una
                // nuova istanza
                if (!map)
                    map = QMUD::qmudWorld->loadInstance(room->instanceEntryPoint() + ".json");

                if (!map)
                {
                    player->sendLine(QObject::tr("Qualcosa non va, contatta un amministratore"));
                    return;
                }

                // La stanza 0 e' l'entrata dell'istanza
                Room* firstRoom = map->room(0);

                if (!firstRoom)
                {
                    player->sendLine(QObject::tr("Qualcosa non va, contatta un amministratore"));
                    return;
                }

                // Entro nell'istanza
                auto chsFrom = ch->room()->characters();

                for (auto chInRoom : chsFrom)
                    if (chInRoom != ch && chInRoom->canSee(ch) && chInRoom->chType() == Character::ChType::CHTYPE_PC)
                        static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 entra in un portale").arg(ch->name(Q_NULLPTR)));

                player->sendLine(QObject::tr(QMUD_GLOBAL_NEW_LINE "&WEntri nel portale e vieni teletrasportato all'entrata di '%1'&d" QMUD_GLOBAL_NEW_LINE).arg(map->name()));
                ch->setParent(firstRoom);
                map->instanceBindPcCharacter(ch->name(Q_NULLPTR));
                static_cast<PcCharacter*>(ch)->sendLook();

                auto chsTo = ch->room()->characters();

                for (auto chInRoom : chsTo)
                    if (chInRoom != ch && chInRoom->canSee(ch) && chInRoom->chType() == Character::ChType::CHTYPE_PC)
                        static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 compare dal nulla").arg(ch->name(Q_NULLPTR)));
            }
        }
        else if (args()[0] == QObject::tr("esci"))
        {
            if (ch->room()->map()->isInstance())
            {
                if (ch->room()->instanceExitRoom())
                {
                    // Esco dall'istanza
                    auto chsFrom = ch->room()->characters();

                    for (auto chInRoom : chsFrom)
                        if (chInRoom != ch && chInRoom->canSee(ch) && chInRoom->chType() == Character::ChType::CHTYPE_PC)
                            static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 entra in un portale").arg(ch->name(Q_NULLPTR)));

                    player->sendLine(QObject::tr(QMUD_GLOBAL_NEW_LINE "&WEntri nel portale e vieni teletrasportato a '%1'&d" QMUD_GLOBAL_NEW_LINE).arg(ch->room()->instanceExitRoom()->name()));
                    ch->setParent(ch->room()->instanceExitRoom());
                    static_cast<PcCharacter*>(ch)->sendLook();

                    auto chsTo = ch->room()->characters();

                    for (auto chInRoom : chsTo)
                        if (chInRoom != ch && chInRoom->canSee(ch) && chInRoom->chType() == Character::ChType::CHTYPE_PC)
                            static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 compare dal nulla").arg(ch->name(Q_NULLPTR)));
                }
                else
                {
                    player->sendLine(QObject::tr("Non c'e' nessuna uscita dall'istanza qui"));
                }
            }
            else
            {
                player->sendLine(QObject::tr("Non sei all'interno di una istanza"));
            }
        }
        else
        {
            sendUsage(player);
        }
    }
    else
    {
        sendUsage(player);
    }
}

void CommandInstance::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandInstance::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Questo MUD mette a disposizione dei giocatori"));
            player->sendLine(QObject::tr("un sistema innovativo di mappe dinamiche chiamate"));
            player->sendLine(QObject::tr("istanze."));
            player->sendLine(QObject::tr("Una istanza e' una mappa creata dal gioco"));
            player->sendLine(QObject::tr("per un giocatore e il suo gruppo."));
            player->sendLine(QObject::tr("Per entrare in una istanza trova uno dei tanti portali"));
            player->sendLine(QObject::tr("sparsi per il mondo."));
            player->sendLine(QObject::tr("Una volta creata, una istanza puo' essere riportata"));
            player->sendLine(QObject::tr("al suo stato originale, tramite il comando 'istanza RESET',"));
            player->sendLine(QObject::tr("dopo che sono trascorsi almeno %1 minuti.").arg(QMUD_GLOBAL_TIMER_REMOVE_INACTIVE_INSTANCE_MS/1000/60));
            player->sendLine(QObject::tr("Se all'interno di una istanza non e' presente nessun"));
            player->sendLine(QObject::tr("giocatore per %1 minuti, il gioco la eliminera'").arg(QMUD_GLOBAL_TIMER_REMOVE_INACTIVE_INSTANCE_MS/1000/60));
            player->sendLine(QObject::tr("automaticamente per inattivita'; quindi, se esci da una"));
            player->sendLine(QObject::tr("istanza, ricordati di rientrarci entro %1 minuti o").arg(QMUD_GLOBAL_TIMER_REMOVE_INACTIVE_INSTANCE_MS/1000/60));
            player->sendLine(QObject::tr("perderai i progressi fatti."));
        }

        player->sendLine(QObject::tr("Uso: istanza                - Lista delle istanze" QMUD_GLOBAL_NEW_LINE
                                     "     istanza entra          - Entra in un portale che conduce" QMUD_GLOBAL_NEW_LINE
                                     "                              all'interno di una istanza" QMUD_GLOBAL_NEW_LINE
                                     "     istanza esci           - Esce da un portale che conduce" QMUD_GLOBAL_NEW_LINE
                                     "                              all'esterno di una istanza" QMUD_GLOBAL_NEW_LINE
                                     "     istanza RESET <indice> - Reset di una istanza"));
    }
}
