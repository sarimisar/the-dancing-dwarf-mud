/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandcrafting.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "globaldebug.h"

using namespace CORE;

CommandCrafting::CommandCrafting(ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandCrafting::~CommandCrafting()
{

}

bool CommandCrafting::isOf(QString cmd, ActionType& action)
{
    if (Command::isOf(QObject::tr("ricette"), cmd) ||
        Command::isOf(QObject::tr("ricetta"), cmd))
    {
        action = ActionType::RECIPES_LIST;
        return true;
    }
    else if (Command::isOf(QObject::tr("professione"), cmd) ||
        Command::isOf(QObject::tr("professioni"), cmd))
    {
        action = ActionType::PROFESSIONS_LIST;
        return true;
    }
    else if (Command::isOf(QObject::tr("crea"), cmd) ||
             Command::isOf(QObject::tr("creo"), cmd))
    {
        action = ActionType::CRAFTING;
        return true;
    }

    return false;
}

QStringList CommandCrafting::commands()
{
    return QStringList() << QObject::tr("ricette")
                         << QObject::tr("professioni")
                         << QObject::tr("creo");
}

void CommandCrafting::executePc(Character* ch, Player* player)
{
    if (m_eAction == ActionType::RECIPES_LIST)
    {
        if (args().size() > 1)
        {
            sendUsage(player);
            return;
        }

        if (ch->isUnconscious())
        {
            player->sendLine(QObject::tr("Sei incoscente."));
            return;
        }

        PcCharacter* pch = static_cast<PcCharacter*>(ch);
        auto recipes = pch->recipes();

        // Divido le recipe per professione
        QMap<QMUD::ProfessionType, QVector<QMUD::Recipe>> recipesByProfession;

        for (auto r : recipes)
        {
            QMUD::Recipe recipe = QMUD::qmudWorld->recipe(r.first, r.second);

            if (recipe.isValid())
                recipesByProfession[recipe.profession()].push_back(recipe);
        }

        // Mostro l'elenco delle recipe conosciute
        if (args().isEmpty())
        {
            bool empty = true;

            for (auto r : recipesByProfession)
            {
                if (!r.isEmpty())
                {
                    for (auto elem : r)
                    {
                        if (elem.level() <= pch->professionLevel(elem.profession()))
                        {
                            if (empty)
                                player->sendLine(QMUD::capitalize(QMUD::professionToReadableString(r[0].profession())) + ":");
                            empty = false;
                            player->sendLine(QObject::tr("  %1.%2 - %3 (livello %4)").arg(elem.mapId()).arg(elem.id()).arg(elem.name()).arg(elem.level()));
                        }
                    }
                }
            }

            if (empty)
                player->sendLine(QObject::tr("Non conosci nessuna ricetta"));
        }
        // Mostro l'elenco delle recipe conosciute di una professione specifica
        // o le informazioni specifiche di una ricetta
        else if (args().size() == 1)
        {
            QMUD::ProfessionType profession = QMUD::ProfessionType::UNKNOWN;

            for (int i = static_cast<int>(QMUD::ProfessionType::UNKNOWN) + 1;
                 i < static_cast<int>(QMUD::ProfessionType::MAX_VALUE); ++i)
                if (QMUD::professionToReadableString(static_cast<QMUD::ProfessionType>(i)).startsWith(args()[0], Qt::CaseInsensitive))
                {
                    profession = static_cast<QMUD::ProfessionType>(i);
                    break;
                }

            // Elenco delle ricette di una professione
            if (profession != QMUD::ProfessionType::UNKNOWN)
            {
                bool empty = true;

                auto recipes = recipesByProfession.find(profession);

                if (recipes != recipesByProfession.end())
                {
                    for (auto elem : recipes.value())
                    {
                        if (elem.level() <= pch->professionLevel(elem.profession()))
                        {
                            if (empty)
                                player->sendLine(QMUD::capitalize(QMUD::professionToReadableString(profession)) + ":");
                            empty = false;
                            player->sendLine(QObject::tr("  %1.%2 - %3 (livello %4)").arg(elem.mapId()).arg(elem.id()).arg(elem.name()).arg(elem.level()));
                        }
                    }
                }

                if (empty)
                    player->sendLine(QObject::tr("Non conosci nessuna ricetta di %1").arg(QMUD::professionToReadableString(profession)));
            }
            // Informazioni su una ricetta
            else
            {
                QStringList argSplit = args()[0].split(".");
                if (argSplit.size() != 2)
                {
                    sendUsage(player);
                    return;
                }

                QMUD::StaticIdType mapId = argSplit[0].toUInt();
                QMUD::StaticIdType recipeId = argSplit[1].toUInt();

                PcCharacter* pch = static_cast<PcCharacter*>(ch);
                auto recipes = pch->recipes();

                QMUD::Recipe recipe;
                for (auto r : recipes)
                {
                    if (r.first == mapId && r.second == recipeId)
                    {
                        recipe = QMUD::qmudWorld->recipe(mapId, recipeId);
                        break;
                    }
                }

                if (!recipe.isValid())
                {
                    sendUsage(player);
                    return;
                }

                player->sendLine(recipe.name());
                player->sendLine(QObject::tr("Indice : %1.%2").arg(mapId).arg(recipeId));
                player->sendLine(QObject::tr("Risorse:"));

                auto resources = recipe.resources();
                for (auto it = resources.begin(); it != resources.end(); ++it)
                {
                    QString strResource = QMUD::resourceToReadableString(it.key());

                    player->sendLine(QString("  %1 x %2")
                                     .arg(it.value())
                                     .arg(QMUD::capitalize(strResource)));
                }
            }
        }
    }
    else if (m_eAction == ActionType::PROFESSIONS_LIST)
    {
        if (args().size() > 1)
        {
            sendUsage(player);
            return;
        }

        if (ch->isUnconscious())
        {
            player->sendLine(QObject::tr("Sei incoscente."));
            return;
        }

        PcCharacter* pch = static_cast<PcCharacter*>(ch);
        auto recipes = pch->recipes();

        // Divido le recipe per professione
        QMap<QMUD::ProfessionType, QVector<QMUD::Recipe>> recipesByProfession;

        for (auto r : recipes)
        {
            QMUD::Recipe recipe = QMUD::qmudWorld->recipe(r.first, r.second);

            if (recipe.isValid())
                recipesByProfession[recipe.profession()].push_back(recipe);
        }

        // Mostro l'elenco delle professioni conosciute
        if (args().isEmpty())
        {
            bool empty = true;

            for (auto r : recipesByProfession)
            {
                if (!r.isEmpty())
                {
                    player->sendLine(QObject::tr("&Y%1 &dlivello &W%2 &d[&W%3 &dricette conosciute]")
                                     .arg(QMUD::capitalize(QMUD::professionToReadableString(r[0].profession())).leftJustified(20))
                            .arg(pch->professionLevel(r[0].profession()))
                            .arg(r.size()));

                    empty = false;
                }
            }

            if (empty)
                player->sendLine(QObject::tr("Non conosci nessuna professione"));
        }
        // Mostro l'elenco delle recipe conosciute di una professione specifica
        // o le informazioni specifiche di una ricetta
        else if (args().size() == 1)
        {
            QMUD::ProfessionType profession = QMUD::ProfessionType::UNKNOWN;

            for (int i = static_cast<int>(QMUD::ProfessionType::UNKNOWN) + 1;
                 i < static_cast<int>(QMUD::ProfessionType::MAX_VALUE); ++i)
                if (QMUD::professionToReadableString(static_cast<QMUD::ProfessionType>(i)).startsWith(args()[0], Qt::CaseInsensitive))
                {
                    profession = static_cast<QMUD::ProfessionType>(i);
                    break;
                }

            // Elenco delle ricette di una professione
            if (profession != QMUD::ProfessionType::UNKNOWN)
            {
                bool empty = true;

                auto recipes = recipesByProfession.find(profession);

                if (recipes != recipesByProfession.end())
                {
                    for (auto elem : recipes.value())
                    {
                        if (elem.level() <= pch->professionLevel(elem.profession()))
                        {
                            if (empty)
                                player->sendLine(QMUD::capitalize(QMUD::professionToReadableString(profession)) + ":");
                            empty = false;
                            player->sendLine(QObject::tr("  %1.%2 - %3 (livello %4)").arg(elem.mapId()).arg(elem.id()).arg(elem.name()).arg(elem.level()));
                        }
                    }
                }

                if (empty)
                    player->sendLine(QObject::tr("Non conosci nessuna ricetta di %1").arg(QMUD::professionToReadableString(profession)));
            }
            // Informazioni su una ricetta
            else
            {
                QStringList argSplit = args()[0].split(".");
                if (argSplit.size() != 2)
                {
                    sendUsage(player);
                    return;
                }

                QMUD::StaticIdType mapId = argSplit[0].toUInt();
                QMUD::StaticIdType recipeId = argSplit[1].toUInt();

                PcCharacter* pch = static_cast<PcCharacter*>(ch);
                auto recipes = pch->recipes();

                QMUD::Recipe recipe;
                for (auto r : recipes)
                {
                    if (r.first == mapId && r.second == recipeId)
                    {
                        recipe = QMUD::qmudWorld->recipe(mapId, recipeId);
                        break;
                    }
                }

                if (!recipe.isValid())
                {
                    sendUsage(player);
                    return;
                }

                player->sendLine(recipe.name());
                player->sendLine(QObject::tr("Indice : %1.%2").arg(mapId).arg(recipeId));
                player->sendLine(QObject::tr("Risorse:"));

                auto resources = recipe.resources();
                for (auto it = resources.begin(); it != resources.end(); ++it)
                {
                    QString strResource = QMUD::resourceToReadableString(it.key());

                    player->sendLine(QString("  %1 x %2")
                                     .arg(it.value())
                                     .arg(QMUD::capitalize(strResource)));
                }
            }
        }
    }
    else if (m_eAction == ActionType::CRAFTING)
    {
        if (args().size() != 1)
        {
            sendUsage(player);
            return;
        }

        QStringList argSplit = args()[0].split(".");
        if (argSplit.size() != 2)
        {
            sendUsage(player);
            return;
        }

        if (ch->isUnconscious())
        {
            player->sendLine(QObject::tr("Sei incoscente."));
            return;
        }

        QString message;
        if (!ch->canMakeAction(message))
        {
            if (player)
                player->sendLine(message);
            return;
        }

        QMUD::StaticIdType mapId = argSplit[0].toUInt();
        QMUD::StaticIdType recipeId = argSplit[1].toUInt();

        PcCharacter* pch = static_cast<PcCharacter*>(ch);
        auto recipes = pch->recipes();

        QMUD::Recipe recipe;
        for (auto r : recipes)
        {
            if (r.first == mapId && r.second == recipeId)
            {
                recipe = QMUD::qmudWorld->recipe(mapId, recipeId);
                break;
            }
        }

        if (!recipe.isValid())
        {
            player->sendLine(QObject::tr("Non conosci la ricetta %1").arg(args()[0]));
            return;
        }

        // Controllo la dimensione dell'inventario nel caso di ricette che prevedono
        // la creazione di un oggetto
        if (recipe.itemId() != QMUD::StaticIdInvalid &&
                ch->inventorySize(true) >= QMUD_GLOBAL_CH_MAX_NUMBER_OF_ITEMS_IN_INVENTORY)
        {
            if (player)
                player->sendLine(QObject::tr("Stai trasportando troppi oggetti."));
            return;
        }

        // Rimuovo le risorse necessarie alla creazione della ricetta
        if (!pch->useResources(recipe.resources()))
        {
            player->sendLine(QObject::tr("Non hai le risorce necessarie"));
            return;
        }

        // Check sul fallimento nella creazione
        if (recipe.level() > pch->professionLevel(recipe.profession()))
        {
            if (QMUD::rollDice(1, pch->professionLevel(recipe.profession()) - recipe.level()) != 1)
            {
                player->sendLine(QObject::tr("&RHai fallito!&d"));

                if (pch->professionLevelInc(recipe.profession()))
                    player->sendLine(QObject::tr("&BLe tue capacita in %1 sono aumentate (+1)&d")
                                     .arg(QMUD::professionToReadableString(recipe.profession())));

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 2, false);
                return;
            }
        }

        // Ricetta per la creazione di una risorsa
        if (recipe.resource() != QMUD::ResourceType::UNKNOWN)
        {
            pch->addResource(recipe.resource(), 1);
        }
        // Ricetta per la creazione di un item
        else if (recipe.itemId() != QMUD::StaticIdInvalid)
        {
            auto map = QMUD::qmudWorld->map(recipe.mapId());

            bool create = false;

            if (map)
            {
                auto itemList = map->mapJsonObjItemsById();
                auto itItemJson = itemList.find(recipe.itemId());
                if (itItemJson != itemList.end())
                {
                    create = !!Item::loadItem(itItemJson.value(), ch, recipe.mapId());
                }
            }

            if (!create)
            {
                // Restituisco le risorse
                auto resources = recipe.resources();

                for (auto it = resources.begin(); it != resources.end(); ++it)
                    pch->addResource(it.key(), it.value());

                // Segnalo il problema
                player->sendLine(QObject::tr("Qualcosa e' andato storto, contatta un amministratore"));
                GLOBAL_WARNING(QObject::tr("Errore nella creazione della ricetta %1:%2")
                              .arg(mapId)
                              .arg(recipeId));
            }
        }
        else
        {
            Q_ASSERT(false);
            player->sendLine(QObject::tr("Qualcosa non va con la ricetta %1, contatta un'amministratore").arg(args()[0]));
            return;
        }

        // Invio l'azione di creazione
        player->sendLine("&G" + recipe.action() + "&d");

        // Lag
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 2, false);

        // Imparo dalla creazione di una ricetta
        if (recipe.level() + 10 > pch->professionLevel(recipe.profession()))
        {
            if (pch->professionLevelInc(recipe.profession()))
                player->sendLine(QObject::tr("&BLe tue capacita in %1 sono aumentate (+1)&d")
                                 .arg(QMUD::professionToReadableString(recipe.profession())));
        }
        else if (recipe.level() + 20 > pch->professionLevel(recipe.profession()))
        {
            if (QMUD::rollDice(1, 2) == 1 && pch->professionLevelInc(recipe.profession()))
                player->sendLine(QObject::tr("&BLe tue capacita in %1 sono aumentate (+1)&d")
                                 .arg(QMUD::professionToReadableString(recipe.profession())));
        }
        else if (recipe.level() + 30 > pch->professionLevel(recipe.profession()))
        {
            if (QMUD::rollDice(1, 4) == 1 && pch->professionLevelInc(recipe.profession()))
                player->sendLine(QObject::tr("&BLe tue capacita in %1 sono aumentate (+1)&d")
                                 .arg(QMUD::professionToReadableString(recipe.profession())));
        }
        else if (recipe.level() + 40 > pch->professionLevel(recipe.profession()))
        {
            if (QMUD::rollDice(1, 8) == 1 && pch->professionLevelInc(recipe.profession()))
                player->sendLine(QObject::tr("&BLe tue capacita in %1 sono aumentate (+1)&d")
                                 .arg(QMUD::professionToReadableString(recipe.profession())));
        }
        else if (recipe.level() + 50 > pch->professionLevel(recipe.profession()))
        {
            if (QMUD::rollDice(1, 12) == 1 && pch->professionLevelInc(recipe.profession()))
                player->sendLine(QObject::tr("&BLe tue capacita in %1 sono aumentate (+1)&d")
                                 .arg(QMUD::professionToReadableString(recipe.profession())));
        }
    }
    else
        sendUsage(player);
}

void CommandCrafting::executeNpc(Character* ch)
{
    Q_UNUSED(ch);
}

void CommandCrafting::sendUsage(Player *player, bool help)
{
    if (help)
        player->sendLine(QObject::tr("Permette di creare un'oggetto usando una delle ricette conosciute"));

    player->sendLine(QObject::tr("Uso: ricette [professione] - mostra l'elenco delle ricette" QMUD_GLOBAL_NEW_LINE
                                 "     ricetta [indice]      - mostra le informazioni relative ad una ricetta specifica" QMUD_GLOBAL_NEW_LINE
                                 "     crea <indice>         - utilizzare una ricetta"));
}
