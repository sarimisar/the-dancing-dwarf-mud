/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandcondition.h"
#include "world.h"
#include "characterclass.h"

using namespace CORE;

CommandCondition::CommandCondition() :
    Command()
{

}

CommandCondition::~CommandCondition()
{

}

bool CommandCondition::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("condizione"), cmd);
}

QStringList CommandCondition::commands()
{
    return QStringList() << QObject::tr("condizione");
}

void CommandCondition::executePc(Character *ch, Player* player)
{
    // Info generali e statistiche
    player->sendLine("&Y--------------------------------------------------------");

    player->sendLine(QObject::tr("&YNome   &d: %1").arg(ch->name(Q_NULLPTR)));
    player->sendLine(QObject::tr("&YSesso  &d: %1").arg(ch->sex() == QMUD::SexType::MALE ? QObject::tr("Maschio") : QObject::tr("Femmina")));
    player->sendLine(QObject::tr("&YRazza  &d: %1").arg(QMUD::Race::toReadableString(ch->race())));
    player->sendLine(QObject::tr("&YClasse &d: %1").arg(ch->chClass()->toReadableString()));
    player->sendLine(QObject::tr("&YLivello&d: %1 (esperienza %2 / %3)").arg(ch->level())
                         .arg(ch->experience())
                         .arg(ch->chClass()->experienceToLevelUp(ch->level())));
    player->sendLine(QObject::tr("&YFor    &d: %1 &YPF&d:%2&Y/&d%3").
                     arg(QString::number(ch->strength()).leftJustified(30)).
                     arg(ch->hitPointsCurrent(), 4).
                     arg(QString::number(ch->hitPointsCurrentMaximum()).leftJustified(4)));
    player->sendLine(QObject::tr("&YCos    &d: %1 &YPA&d:%2&Y/&d%3").
                     arg(QString::number(ch->constitution()).leftJustified(30)).
                     arg(ch->actionPointsCurrent(), 4).
                     arg(QString::number(ch->actionPointsCurrentMaximum()).leftJustified(4)));
    player->sendLine(QObject::tr("&YInt    &d: %1 &YPM&d:%2&Y/&d%3").
                     arg(QString::number(ch->intelligence()).leftJustified(30)).
                     arg(ch->manaPointsCurrent(), 4).
                     arg(QString::number(ch->manaPointsCurrentMaximum()).leftJustified(4)));
    player->sendLine(QObject::tr("&YSag    &d: %1 &YClasse armatura&d: %2").
                     arg(QString::number(ch->wisdom()).leftJustified(30)).
                     arg(QString::number(ch->armorClass())));
    player->sendLine(QObject::tr("&YDex    &d: %1 &YBonus colpire&d  : %2").
                     arg(QString::number(ch->dexterity()).leftJustified(30)).
                     arg(QString::number(ch->hitBonus())));
    player->sendLine(QObject::tr("&YCar    &d: %1 &YBonus danno&d    : %2").
                     arg(QString::number(ch->charisma()).leftJustified(30)).
                     arg(QString::number(ch->damageBonus())));
    player->sendLine(QObject::tr("&YAurei  &d: %1").arg(QMUD::coinsToReadableString(ch->coins())));

    bool sendLine = true;

    // Cibo, acqua e tonici
    if (!ch->currentTonic().isEmpty() ||
            ch->currentFoodIsEating() ||
            ch->currentDrinkIsDrinking())
    {
        player->sendLine("&Y------------------------ STATO -------------------------&d");
        sendLine = false;

        if (!ch->currentTonic().isEmpty())
        {
            player->sendLine(QObject::tr("Hai assunto il tonico %1 (scade tra %2)").arg(ch->currentTonic()).arg(QMUD::minutesToString(QMUD_GLOBAL_MINUTES_FROM_TURNS(ch->currentTonicRounds()))));
        }

        if (!ch->currentFood().isEmpty())
            player->sendLine(QObject::tr("Stai mangiando %1").arg(ch->currentFood()));

        if (!ch->currentDrink().isEmpty())
            player->sendLine(QObject::tr("Stai bevendo %1").arg(ch->currentDrink()));
    }

    // Status
    auto status = ch->temporaryStatus();


    for (auto it = status.begin(); it != status.end(); ++it)
    {
        if (it.value() > 0)
        {
            if (sendLine)
            {
                player->sendLine("&Y------------------------ STATO -------------------------&d");
                sendLine = false;
            }

            if (it.key() == QMUD::TemporaryStatusType::BLINDED)
                player->sendLine(QObject::tr("&RSei accecato (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::STUNNED)
                player->sendLine(QObject::tr("&RSei stordito (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::SHIELD)
                player->sendLine(QObject::tr("&WIncantesimo scudo (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::DIVINE_SHIELD)
                player->sendLine(QObject::tr("&WIncantesimo scudo divino (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::FLY)
                player->sendLine(QObject::tr("&WIncantesimo volo (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::INVISIBILITY)
                player->sendLine(QObject::tr("&WIncantesimo invisibilita (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::SEE_INVISIBILITY)
                player->sendLine(QObject::tr("&WIncantesimo vedi invisibile (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::SANCTIFY)
                player->sendLine(QObject::tr("&WIncantesimo santificazione (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::STRENGHT)
                player->sendLine(QObject::tr("&WIncantesimo forza (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::ICE_SHIELD)
                player->sendLine(QObject::tr("&WIncantesimo scudo di ghiaccio (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::FIRE_SHIELD)
                player->sendLine(QObject::tr("&WIncantesimo scudo di fuoco (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::BLADES_SHIELD)
                player->sendLine(QObject::tr("&WIncantesimo scudo di lame (scade tra %1)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1)
                player->sendLine(QObject::tr("&WIncantesimo immagini illusorie [1] (scade tra %1 minuti)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2)
                player->sendLine(QObject::tr("&WIncantesimo immagini illusorie [2] (scade tra %1 minuti)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3)
                player->sendLine(QObject::tr("&WIncantesimo immagini illusorie [3] (scade tra %1 minuti)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4)
                player->sendLine(QObject::tr("&WIncantesimo immagini illusorie [4] (scade tra %1 minuti)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
            else if (it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5)
                player->sendLine(QObject::tr("&WIncantesimo immagini illusorie [5] (scade tra %1 minuti)&d").arg(QMUD_GLOBAL_MINUTES_FROM_TURNS(it.value())));
        }
    }

    player->sendLine("&Y--------------------------------------------------------&d");
}

void CommandCondition::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandCondition::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Mostra il tuo stato corrente"));
        }

        player->sendLine(QObject::tr("Uso: condizione"));
    }
}
