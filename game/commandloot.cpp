/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandloot.h"
#include "world.h"
#include "item.h"

using namespace CORE;

CommandLoot::CommandLoot() :
    Command()
{

}

CommandLoot::~CommandLoot()
{

}

bool CommandLoot::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("bottino"), cmd);
}

QStringList CommandLoot::commands()
{
    return QStringList() << QObject::tr("bottino");
}

void CommandLoot::executePc(Character *ch, Player* player)
{
    if (args().isEmpty())
    {
        PcCharacter* pch = static_cast<PcCharacter*>(ch);

        player->sendLine(QObject::tr("&Y------------ Bottino ------------&d"));

        bool empty = true;
        int index = 0;
        qint64 coins = 0;
        for (auto& e : pch->loot())
        {
            if (!e.toSell())
            {
                empty = false;
                player->sendLine(QString("%1 - %2%3&d (%4)")
                                 .arg(index)
                                 .arg(QMUD::itemQualityToColor(e.quality()))
                                 .arg(e.name())
                                 .arg(QMUD::coinsToReadableString(e.coinsValue())));
                coins += e.coinsValue();
            }

            index++;
        }

        if (empty)
        {
            player->sendLine(QObject::tr("Il tuo bottino e' vuoto"));
            return;
        }
        else
        {
            player->sendLine(QObject::tr("Il valore totale e' %1").arg(QMUD::coinsToReadableString(coins)));
        }
    }
    else if (args().size() == 1)
    {
        if (QString("cestino").startsWith(args()[0]))
        {
            PcCharacter* pch = static_cast<PcCharacter*>(ch);

            player->sendLine(QObject::tr("&g------------ Scartati ------------&d"));

            bool empty = true;
            int index = 0;
            qint64 coins = 0;
            for (auto& e : pch->loot())
            {
                if (e.toSell())
                {
                    empty = false;
                    player->sendLine(QString("%1 - %2%3&d (%4)")
                                     .arg(index)
                                     .arg(QMUD::itemQualityToColor(e.quality()))
                                     .arg(e.name())
                                     .arg(QMUD::coinsToReadableString(e.coinsValue())));
                    coins += e.coinsValue();
                }

                index++;
            }

            if (empty)
            {
                player->sendLine(QObject::tr("Non hai scartato nulla"));
                return;
            }
            else
            {
                player->sendLine(QObject::tr("Il valore totale e' %1").arg(QMUD::coinsToReadableString(coins)));
            }
        }
        else if (args()[0] == "SACRIFICA-CESTINO")
        {
            PcCharacter* pch = static_cast<PcCharacter*>(ch);

            bool empty = true;
            qint64 coins = 0;
            for (auto it = pch->loot().begin(); it != pch->loot().end(); )
            {
                if (it->toSell())
                {
                    empty = false;
                    player->sendLine(QObject::tr("Sacrifichi %1 agli dei. Vieni premiato con %2")
                                     .arg(it->name())
                                     .arg(QMUD::coinsToReadableString(it->coinsValue() / 10)));
                    coins += it->coinsValue() / 10;

                    it = pch->loot().erase(it);
                }
                else
                    ++it;
            }

            if (empty)
            {
                player->sendLine(QObject::tr("Non hai nulla da sacrificare agli dei"));
                return;
            }
            else
            {
                player->sendLine(QObject::tr("Hai guadagnato %1").arg(QMUD::coinsToReadableString(coins)));
                pch->addCoins(coins);
                pch->sendDataLoot();
            }
        }
        else if (args()[0] == "SCARTA-TUTTO")
        {
            PcCharacter* pch = static_cast<PcCharacter*>(ch);

            bool empty = true;

            for (auto it = pch->loot().begin(); it != pch->loot().end(); )
            {
                if (!it->toSell())
                {
                    empty = false;
                    player->sendLine(QObject::tr("Scarti l'oggetto %1%2&d")
                                     .arg(QMUD::itemQualityToColor(it->quality()))
                                     .arg(it->name()));

                    it->setToSell(true);
                }
                else
                    ++it;
            }

            if (empty)
            {
                player->sendLine(QObject::tr("Non hai nulla da scartare"));
                return;
            }
            else
            {
                pch->sendDataLoot();
            }
        }
        else
        {
            sendUsage(player);
        }
    }
    else if (args().size() == 2)
    {
        PcCharacter* pch = static_cast<PcCharacter*>(ch);

        if (QString("tieni").startsWith(args()[0]))
        {
            int index = args()[1].toInt();

            if (index < 0 || index >= pch->loot().size())
            {
                player->sendLine(QObject::tr("L'indice %1 non e' valido").arg(index));
                return;
            }

            if (pch->loot()[index].toSell())
            {
                player->sendLine(QObject::tr("L'oggetto %1 - %2%3&d e' nel cestino"
                                             QMUD_GLOBAL_NEW_LINE
                                             "Puoi recuperarlo con il comando: bottino recupera %1")
                                 .arg(index)
                                 .arg(QMUD::itemQualityToColor(pch->loot()[index].quality()))
                                 .arg(pch->loot()[index].name()));
                return;
            }

            if (ch->inventorySize(true) >= QMUD_GLOBAL_CH_MAX_NUMBER_OF_ITEMS_IN_INVENTORY)
            {
                if (player)
                    player->sendLine(QObject::tr("Stai trasportando troppi oggetti."));
                return;
            }

            Item *item = Q_NULLPTR;

            if (pch->loot()[index].randomCode().isEmpty())
                item = Item::loadItem(pch->loot()[index].json(), ch, pch->loot()[index].mapStaticId());
            else
                item = Item::createFromRandomCode(pch->loot()[index].randomCode(), ch);

            if (!item)
            {
                player->sendLine(QObject::tr("Qualcosa non va"));
                return;
            }

            item->setIsBind(true);

            player->sendLine(QObject::tr("Tieni l'oggetto %1 - %2%3&d")
                             .arg(index)
                             .arg(QMUD::itemQualityToColor(pch->loot()[index].quality()))
                             .arg(pch->loot()[index].name()));

            pch->loot().remove(index);

            pch->sendDataLoot();
        }
        else if (QString("scarta").startsWith(args()[0]))
        {
            int index = args()[1].toInt();

            if (index < 0 || index >= pch->loot().size())
            {
                player->sendLine(QObject::tr("L'indice %1 non e' valido").arg(index));
                return;
            }

            if (pch->loot()[index].toSell())
            {
                player->sendLine(QObject::tr("Hai gia' scartato %1 - %2").arg(index).arg(pch->loot()[index].name()));
                return;
            }

            player->sendLine(QObject::tr("Scarti l'oggetto %1 - %2%3&d")
                             .arg(index)
                             .arg(QMUD::itemQualityToColor(pch->loot()[index].quality()))
                             .arg(pch->loot()[index].name()));
            pch->loot()[index].setToSell(true);
            pch->sendDataLoot();
        }
        else if (QString("recupera").startsWith(args()[0]))
        {
            int index = args()[1].toInt();

            if (index < 0 || index >= pch->loot().size())
            {
                player->sendLine(QObject::tr("L'indice %1 non e' valido").arg(index));
                return;
            }

            if (!pch->loot()[index].toSell())
            {
                player->sendLine(QObject::tr("L'oggetto %1 - %2%3&d non e' nel cestino")
                                 .arg(index)
                                 .arg(QMUD::itemQualityToColor(pch->loot()[index].quality()))
                                 .arg(pch->loot()[index].name()));
                return;
            }

            player->sendLine(QObject::tr("Recuperi l'oggetto %1 - %2%3&d")
                             .arg(index)
                             .arg(QMUD::itemQualityToColor(pch->loot()[index].quality()))
                             .arg(pch->loot()[index].name()));
            pch->loot()[index].setToSell(false);
            pch->sendDataLoot();
        }
    }
    else
    {
        sendUsage(player);
    }
}

void CommandLoot::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandLoot::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: bottino tieni <indice>    : Tiene l'oggetto specificato" QMUD_GLOBAL_NEW_LINE
                                 "     bottino scarta <indice>   : Cestina l'oggetto specificato" QMUD_GLOBAL_NEW_LINE
                                 "     bottino recupera <indice> : Recupera dal cestino l'oggetto specificato" QMUD_GLOBAL_NEW_LINE
                                 "     bottino cestino           : Mostra l'elenco degli oggetti nel cestino" QMUD_GLOBAL_NEW_LINE
                                 "     bottino SACRIFICA-CESTINO : Sacrifica tutti gli oggetti del cestino" QMUD_GLOBAL_NEW_LINE
                                 "     bottino SCARTA-TUTTO      : Scarta tutti gli oggetti del bottino"));
}
