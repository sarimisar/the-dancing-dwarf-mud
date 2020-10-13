/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandeat.h"
#include "itemfood.h"
#include "itemdrink.h"
#include "itempotion.h"
#include "itemtonic.h"
#include "room.h"

using namespace CORE;

CommandEat::CommandEat(ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandEat::~CommandEat()
{

}

bool CommandEat::isOf(QString cmd, ActionType &action)
{
    if (Command::isOf(QObject::tr("mangia"), cmd) ||
            Command::isOf(QObject::tr("mangio"), cmd))
    {
        action = ActionType::EAT;
        return true;
    }
    else if (Command::isOf(QObject::tr("bevo"), cmd) ||
            Command::isOf(QObject::tr("bevi"), cmd))
    {
        action = ActionType::DRINK;
        return true;
    }

    return false;
}

QStringList CommandEat::commands()
{
    return QStringList() << QObject::tr("mangio")
                         << QObject::tr("bevo");
}

void CommandEat::executePc(Character* ch, Player* player)
{
    if (args().size() != 1)
    {
        if (player)
            sendUsage(player);
        return;
    }

    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    // Controllo se posso agire
    QString message;

    if (!ch->canMakeAction(message))
    {
        if (player)
            player->sendLine(message);

        return;
    }

    bool wrongUsage = false;
    Item* item = findItemInInventory(args()[0], ch->inventory(), wrongUsage);

    if (wrongUsage)
    {
        if (player)
            sendUsage(player);
        return;
    }

    if (!item)
    {
        if (player)
            player->sendLine(QObject::tr("Non hai nessun '%1'").arg(args()[0]));
        return;
    }

    // Controllo se sono in combattimento
    if (ch->isInCombat())
    {
        if (!(m_eAction == ActionType::DRINK && item->itemType() == QMUD::ItemType::POTION))
        {
            if (player)
                player->sendLine(QObject::tr("Non puoi mentre stai combattendo"));

            return;
        }
    }

    if (m_eAction == ActionType::EAT)
    {
        if (item->itemType() != QMUD::ItemType::FOOD)
        {
            if (player)
                player->sendLine(QObject::tr("Non puoi mangiare %1").arg(item->name()));
            return;
        }

        if (item->level() > ch->level())
        {
            if (player)
                player->sendLine(QObject::tr("Non hai il livello adatto per mangiare %1").arg(item->name()));
            return;
        }

        if (!ch->currentFood().isEmpty())
        {
            if (player)
			{
				if (ch->currentFoodIsEating())
					player->sendLine(QObject::tr("Stai gia' mangiando %1").arg(ch->currentFood()));
				else
					player->sendLine(QObject::tr("Troppo presto, hai appena finito di mangiare %1").arg(ch->currentFood()));
			}

            return;
        }

        ItemFood* food = static_cast<ItemFood*>(item);

        if (player)
            player->sendLine(QObject::tr("Inizi a mangiare %1").arg(item->name()));

        auto characters = ch->room()->characters();

        for (auto chInRoom : characters)
            if (chInRoom != ch && chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(ch))
                static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 inizia a mangiare %2").arg(ch->name(Q_NULLPTR)).arg(item->name()));

        ch->setCurrentFood(food->name(), 10, food->hpGainBonus(), food->apGainBonus());
        food->destroy();
    }
    else if (m_eAction == ActionType::DRINK)
    {
        if (item->itemType() != QMUD::ItemType::DRINK &&
            item->itemType() != QMUD::ItemType::POTION &&
            item->itemType() != QMUD::ItemType::TONIC)
        {
            if (player)
                player->sendLine(QObject::tr("Non puoi bere %1").arg(item->name()));
            return;
        }

        if (item->level() > ch->level())
        {
            if (player)
                player->sendLine(QObject::tr("Non hai il livello adatto per bere %1").arg(item->name()));
            return;
        }

        if (item->itemType() == QMUD::ItemType::DRINK)
        {
            if (!ch->currentDrink().isEmpty())
            {
                if (player)
                {
                    if (ch->currentDrinkIsDrinking())
                        player->sendLine(QObject::tr("Stai gia' bevendo %1").arg(ch->currentDrink()));
                    else
                        player->sendLine(QObject::tr("Troppo presto, hai appena finito di bere %1").arg(ch->currentDrink()));
                }

                return;
            }

            ItemDrink* drink = static_cast<ItemDrink*>(item);

            if (player)
                player->sendLine(QObject::tr("Inizi a bere %1").arg(item->name()));

            auto characters = ch->room()->characters();

            for (auto chInRoom : characters)
                if (chInRoom != ch && chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(ch))
                    static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 inizia a bere %2").arg(ch->name(Q_NULLPTR)).arg(item->name()));

            ch->setCurrentDrink(drink->name(), 10, drink->mpGainBonus());
            drink->destroy();
        }
        else if (item->itemType() == QMUD::ItemType::POTION)
        {
            ItemPotion* potion = static_cast<ItemPotion*>(item);

            QString message;

            if (!ch->canMakeAction(message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            if (item->level() > ch->level())
            {
                if (player)
                    player->sendLine(QObject::tr("Non hai il livello adatto per bere %1").arg(item->name()));
                return;
            }

            // Bevo la pozione
            if (player)
                player->sendLine(QObject::tr("Bevi %1").arg(item->name()));

            auto characters = ch->room()->characters();

            for (auto chInRoom : characters)
                if (chInRoom != ch && chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(ch))
                    static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 beve %2").arg(ch->name(Q_NULLPTR)).arg(item->name()));

            // Guadagno HP/MP e AP
            if (potion->hitPoints() > 0)
            {
                ch->healingHitPoints(potion->hitPoints());
                if (player)
                    player->sendLine(QObject::tr("&RGuadagni %1 punti vita&d").arg(potion->hitPoints()));
            }

            if (potion->manaPoints() > 0)
            {
                ch->healingManaPoints(potion->manaPoints());
                if (player)
                    player->sendLine(QObject::tr("&CGuadagni %1 punti mana&d").arg(potion->manaPoints()));
            }

            if (potion->actionPoints() > 0)
            {
                ch->healingActionPoints(potion->actionPoints());
                if (player)
                    player->sendLine(QObject::tr("&WGuadagni %1 punti azione&d").arg(potion->actionPoints()));
            }

            // Setto gli status
            auto statusList = potion->baseTemporaryStatus().temporaryStatus();

            for (auto status : statusList)
                ch->setTemporaryStatus(status, QMUD_GLOBAL_TURNS_FROM_SECONDS(potion->durationSeconds()), true);

            // Setto la lag
            ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 2, false);

            potion->destroy();
        }
        else if (item->itemType() == QMUD::ItemType::TONIC)
        {
            ItemTonic* tonic = static_cast<ItemTonic*>(item);

            QString message;

            if (!ch->canMakeAction(message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            if (item->level() > ch->level())
            {
                if (player)
                    player->sendLine(QObject::tr("Non hai il livello adatto per bere %1").arg(item->name()));
                return;
            }

            // Bevo il tonico
            if (player)
                player->sendLine(QObject::tr("Bevi %1").arg(item->name()));

            auto characters = ch->room()->characters();

            for (auto chInRoom : characters)
                if (chInRoom != ch && chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(ch))
                    static_cast<PcCharacter*>(chInRoom)->player()->sendLine(QObject::tr("%1 beve %2").arg(ch->name(Q_NULLPTR)).arg(item->name()));

            // Setto il tonico corrente
            ch->setCurrentTonic(item->name(), QMUD_GLOBAL_TURNS_FROM_SECONDS(tonic->durationSeconds()), tonic->baseBonus());

            // Setto la lag
            ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 2, false);
            tonic->destroy();
        }
    }
}

void CommandEat::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandEat::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: mangio/bevo [<n>.]<nome>"));
}
