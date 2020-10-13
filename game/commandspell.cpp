/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandspell.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "characterclass.h"

using namespace CORE;

CommandSpell::CommandSpell(ActionType type) :
    Command(),
    m_eActionType(type)
{

}

CommandSpell::~CommandSpell()
{

}

bool CommandSpell::isOf(QString cmd, ActionType& type)
{
    if (Command::isOf(QObject::tr("formulo"), cmd) ||
            Command::isOf(QObject::tr("formula"), cmd))
    {
        type = ActionType::SPELL;
        return true;
    }
    else if (Command::isOf(QObject::tr("incantesimi"), cmd))
    {
        type = ActionType::SPELL_LIST;
        return true;
    }

    return false;
}

QStringList CommandSpell::commands()
{
    return QStringList() << QObject::tr("formulo")
                         << QObject::tr("incantesimi");
}

void CommandSpell::executePc(Character* ch, Player* player)
{
    if (m_eActionType == ActionType::SPELL)
    {
        if (args().size() == 0)
        {
            sendUsage(player);
            return;
        }

        if (ch->isUnconscious())
        {
            if (player)
                player->sendLine(QObject::tr("Sei incoscente."));

            return;
        }

        // Identifico lo spell
        QMUD::SpellType spell;
        if ((spell = QMUD::spellFromString(args()[0])) == QMUD::SpellType::UNKNOWN)
        {
            if (player)
                player->sendLine(QObject::tr("%1 non e' un incantesimo valido").arg(args()[0]));
            return;
        }

        // Identifico il target
        Entity* target = Q_NULLPTR;

        if (args().size() >= 2)
        {
            if (args()[1].compare(QObject::tr("me"), Qt::CaseInsensitive) == 0)
            {
                target = ch;
            }
            else
            {
                bool wrongUsage = false;

                target = findTargetInRoom(args()[1], wrongUsage, true);

                if (wrongUsage)
                {
                    sendUsage(player);
                    return;
                }
            }
        }

        // Controllo se il character puo' compiere una azione
        QString message;
        if (!ch->canMakeAction(message))
        {
            if (player)
                player->sendLine(message);
            return;
        }

        // Casto lo spell
        if (spell == QMUD::SpellType::MAGIC_MISSILE)
            spellMagicMissile(ch, player, target);
        else if (spell == QMUD::SpellType::SHIELD)
            spellShield(ch, player, target);
        else if (spell == QMUD::SpellType::SANCTIFY)
            spellSanctify(ch, player, target);
        else if (spell == QMUD::SpellType::STRENGHT)
            spellStrenght(ch, player, target);
        else if (spell == QMUD::SpellType::CURE)
            spellCure(ch, player, target);
        else if (spell == QMUD::SpellType::FIREBALL)
            spellFireball(ch, player);
        else if (spell == QMUD::SpellType::BURNING_HANDS)
            spellBurningHands(ch, player);
        else if (spell == QMUD::SpellType::DIVINE_SHIELD)
            spellDivineShield(ch, player, target);
        else if (spell == QMUD::SpellType::INFLICTING_WOUNDS)
            spellInflictingWounds(ch, player, target);
        else if (spell == QMUD::SpellType::FLY)
            spellFly(ch, player, target);
        else if (spell == QMUD::SpellType::INVISIBILITY)
            spellInvisibility(ch, player, target);
        else if (spell == QMUD::SpellType::SEE_INVISIBILITY)
            spellSeeInvisibility(ch, player, target);
        else if (spell == QMUD::SpellType::FIRE_SHIELD)
            spellFireShield(ch, player, target);
        else if (spell == QMUD::SpellType::ICE_SHIELD)
            spellIceShield(ch, player, target);
        else if (spell == QMUD::SpellType::BLADES_SHIELD)
            spellBladesShield(ch, player, target);
        else if (spell == QMUD::SpellType::FIERY_ARROW)
            spellFieryArrow(ch, player, target);
        else if (spell == QMUD::SpellType::ICE_STORM)
            spellIceStorm(ch, player);
        else if (spell == QMUD::SpellType::ILLUSORY_IMAGES)
            spellIllusoryImages(ch, player, target);
        else
        {
            Q_ASSERT(false);

            sendUsage(player);
            return;
        }
    }
    else if (m_eActionType == ActionType::SPELL_LIST)
    {
        if (player)
        {
            QString spellList;

            for (int i = static_cast<int>(QMUD::SpellType::UNKNOWN); i < static_cast<int>(QMUD::SpellType::MAX_VALUE); ++i)
                if (ch->canUseSpell(static_cast<QMUD::SpellType>(i)))
                    spellList += QMUD_GLOBAL_NEW_LINE + QString("  ") + QMUD::spellToReadableString(static_cast<QMUD::SpellType>(i));

            if (!spellList.isEmpty())
            {
                player->sendLine(QObject::tr("Conosci i seguenti incantesimi:"));
                player->sendLine(spellList);
            }
            else
            {
                player->sendLine(QObject::tr("Non conosci nessun incantesimo"));
            }
        }
    }
}

void CommandSpell::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandSpell::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    if (player)
        player->sendLine(QObject::tr("Uso: formula <incantesimo> [target]"));
}

void CommandSpell::spellMagicMissile(Character *ch, Player *player, Entity *target)
{
    int baseLevel = qMax(ch->level(), 2);
    int statBonus = qMax(0, ch->intelligence());
    int damage = QMUD::rollDice(baseLevel, 4) + statBonus * baseLevel / 2;

    spellDamageTarget(ch, player, target, QMUD::SpellType::MAGIC_MISSILE, QMUD::DamageType::ENERGY, qMin(ch->manaPointsCurrentMaximum() / 10, 25), damage, 1,
                      QObject::tr("&WColpisci %1 con il tuo dardo incantato&d"),
                      QObject::tr("&W%1 ti colpisce con un dardo incantato&d"),
                      QObject::tr("&W%1 colpisce %2 con un dardo incantato&d"));
}

void CommandSpell::spellShield(Character *ch, Player *player, Entity *target)
{
    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusTarget(ch, player, target, QMUD::SpellType::SHIELD, QMUD::TemporaryStatusType::SHIELD,
                         25, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(qMax(5, ch->level()/2)), 2,
                         QObject::tr("Lanci un incantesimo su %1"),
                         QObject::tr("%1 lancia un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su %2"),
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellStrenght(Character *ch, Player *player, Entity *target)
{
    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusTarget(ch, player, target, QMUD::SpellType::STRENGHT, QMUD::TemporaryStatusType::STRENGHT,
                         25, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(qMax(5, ch->level()/2)), 2,
                         QObject::tr("Lanci un incantesimo su %1"),
                         QObject::tr("%1 lancia un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su %2"),
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellIllusoryImages(Character *ch, Player *player, Entity *target)
{
    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    if (target && ch != target)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return;
    }

    int numberOfImages = qMax(1, ch->level() / 20);

    QMUD::TemporaryStatusType imagesStatus = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1;
    if (numberOfImages == 2)
        imagesStatus = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2;
    else if (numberOfImages == 3)
        imagesStatus = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3;
    else if (numberOfImages == 4)
        imagesStatus = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4;
    else if (numberOfImages == 5)
        imagesStatus = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5;

    spellAddStatusTarget(ch, player, target, QMUD::SpellType::ILLUSORY_IMAGES, imagesStatus,
                         100, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(10), 2,
                         QObject::tr("Lanci un incantesimo su %1"),
                         QObject::tr("%1 lancia un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su %2"),
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));

    ch->illusoryImagesResetHP();
}

void CommandSpell::spellFireball(Character *ch, Player *player)
{
    int baseLevel = qMax(ch->level(), 2);
    int statBonus = qMax(0, ch->intelligence());
    int damage = QMUD::rollDice(baseLevel, 6) + statBonus * baseLevel / 2;

    spellDamageZone(ch, player, QMUD::SpellType::FIREBALL, QMUD::DamageType::FIRE, qMin(ch->manaPointsCurrentMaximum() / 4, 100), damage, 2,
                    QObject::tr("&RColpisci %1 con una palla di fuoco&d"),
                    QObject::tr("&R%1 ti colpisce con una palla di fuoco&d"),
                    QObject::tr("&R%1 colpisce %2 con una palla di fuoco&d"));
}

void CommandSpell::spellBurningHands(Character *ch, Player *player)
{
    int baseLevel = qMax(ch->level(), 4);
    int statBonus = qMax(0, qMax(ch->intelligence(), ch->wisdom()));
    int damage = QMUD::rollDice(baseLevel, 4) + statBonus * baseLevel / 4;

    spellDamageZone(ch, player, QMUD::SpellType::BURNING_HANDS, QMUD::DamageType::FIRE, qMin(ch->manaPointsCurrentMaximum() / 10, 50), damage, 1,
                    QObject::tr("&RLe fiamme che escono dalle tue mani colpiscono %1&d"),
                    QObject::tr("&RLe fiamme che escono dalle mani di %1 ti colpiscono&d"),
                    QObject::tr("&RLe fiamme che escono dalle mani di %1 colpiscono %2&d"));
}

void CommandSpell::spellFly(Character *ch, Player *player, Entity *target)
{
    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusTarget(ch, player, target, QMUD::SpellType::FLY, QMUD::TemporaryStatusType::FLY,
                         25, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(qMax(5, ch->level()/2)), 1,
                         QObject::tr("Lanci un incantesimo su %1"),
                         QObject::tr("%1 lancia un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su %2"),
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellInvisibility(Character *ch, Player *player, Entity *target)
{
    spellAddStatusTarget(ch, player, target, QMUD::SpellType::INVISIBILITY, QMUD::TemporaryStatusType::INVISIBILITY,
                         25, QMUD_GLOBAL_TURNS_FROM_MINUTES(qMax(5, ch->level()/4)), 2,
                         QObject::tr("Lanci un incantesimo su %1"),
                         QObject::tr("%1 lancia un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su %2"),
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellSeeInvisibility(Character *ch, Player *player, Entity *target)
{
    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusTarget(ch, player, target, QMUD::SpellType::SEE_INVISIBILITY, QMUD::TemporaryStatusType::SEE_INVISIBILITY,
                         255, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(qMax(5, ch->level()/4)), 2,
                         QObject::tr("Lanci un incantesimo su %1"),
                         QObject::tr("%1 lancia un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su %2"),
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellFireShield(Character *ch, Player *player, Entity *target)
{
    if (target && ch != target)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return;
    }

    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusShield(ch, player, target, QMUD::SpellType::FIRE_SHIELD, QMUD::TemporaryStatusType::FIRE_SHIELD,
                         200, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(5), 2,
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellIceShield(Character *ch, Player *player, Entity *target)
{
    if (target && ch != target)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return;
    }

    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusShield(ch, player, target, QMUD::SpellType::FIRE_SHIELD, QMUD::TemporaryStatusType::ICE_SHIELD,
                         200, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(5), 2,
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellBladesShield(Character *ch, Player *player, Entity *target)
{
    if (target && ch != target)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return;
    }

    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusShield(ch, player, target, QMUD::SpellType::FIRE_SHIELD, QMUD::TemporaryStatusType::BLADES_SHIELD,
                         200, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(5), 2,
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellFieryArrow(Character *ch, Player *player, Entity *target)
{
    int baseLevel = qMax(ch->level(), 2);
    int statBonus = qMax(0, qMax(ch->intelligence(), ch->wisdom()));
    int damage = QMUD::rollDice(baseLevel, 6) + statBonus * baseLevel / 2;

    spellDamageTarget(ch, player, target, QMUD::SpellType::MAGIC_MISSILE, QMUD::DamageType::FIRE, qMin(ch->manaPointsCurrentMaximum() / 10, 50), damage, 1,
                      QObject::tr("&RColpisci %1 con la tua freccia infuocata&d"),
                      QObject::tr("&R%1 ti colpisce con una freccia infuocata&d"),
                      QObject::tr("&R%1 colpisce %2 con una freccia infuocata&d"));
}

void CommandSpell::spellIceStorm(Character *ch, Player *player)
{
    int baseLevel = qMax(ch->level(), 2);
    int statBonus = qMax(0, ch->intelligence());
    int damage = QMUD::rollDice(baseLevel, 4) + statBonus * baseLevel / 2;

    spellDamageZone(ch, player, QMUD::SpellType::ICE_STORM, QMUD::DamageType::ICE, qMin(ch->manaPointsCurrentMaximum() / 4, 150), damage, 1,
                    QObject::tr("&CLa tempesta di ghiaccio che hai evocato colpisce %1&d"),
                    QObject::tr("&CLa tempesta di ghiaccio evocata da %1 ti colpisce&d"),
                    QObject::tr("&CLa tempesta di ghiaccio evocata da %1 colpisce %2&d"));
}

void CommandSpell::spellSanctify(Character *ch, Player *player, Entity *target)
{
    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusTarget(ch, player, target, QMUD::SpellType::SANCTIFY, QMUD::TemporaryStatusType::SANCTIFY,
                         100, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(5), 2,
                         QObject::tr("Lanci un incantesimo su %1"),
                         QObject::tr("%1 lancia un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su %2"),
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellCure(Character *ch, Player *player, Entity *target)
{
    int mana = qMin(ch->manaPointsCurrentMaximum() / 10, 50);

    spellCureTarget(ch, player, target, QMUD::SpellType::CURE, mana,
                    QMUD::rollDice(qMax(1, ch->level()/2), 6), 1,
                    QObject::tr("&WInvochi il potere degli dei per curare le ferite di %1&d"),
                    QObject::tr("&W%1 invoca il potere degli dei per curare le tue ferite&d"),
                    QObject::tr("&W%1 invoca il potere degli dei per curare le ferite di %2&d"),
                    QObject::tr("&WInvochi il potere degli dei per curare le tue ferite&d"),
                    QObject::tr("&W%1 invoca il potere degli dei per curare le sue ferite&d"));
}

void CommandSpell::spellDivineShield(Character *ch, Player *player, Entity *target)
{
    if (target && ch != target)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return;
    }

    bool isNpc = ch->chType() == Character::ChType::CHTYPE_NPC && (target == ch || !target);

    spellAddStatusShield(ch, player, target, QMUD::SpellType::FIRE_SHIELD, QMUD::TemporaryStatusType::DIVINE_SHIELD,
                         200, isNpc ? QMUD_GLOBAL_STATUS_DURATION_INFINITE : QMUD_GLOBAL_TURNS_FROM_MINUTES(5), 2,
                         QObject::tr("Lanci un incantesimo su di te"),
                         QObject::tr("%1 lancia un incantesimo su di se"));
}

void CommandSpell::spellInflictingWounds(Character *ch, Player *player, Entity *target)
{
    int mana = qMin(ch->manaPointsCurrentMaximum() / 10, 50);

    int baseLevel = qMax(ch->level(), 2);
    int statBonus = qMax(0, ch->wisdom());
    int damage = QMUD::rollDice(baseLevel, 2) + statBonus * baseLevel / 2;

    spellDamageTarget(ch, player, target, QMUD::SpellType::INFLICTING_WOUNDS, QMUD::DamageType::DIVINE, mana, damage, 1,
              QObject::tr("&pLa forza oscura proveniente dal tuo tocco ferisce %1&d"),
              QObject::tr("&pLa forza oscura proveniente dal tocco di %1 ti ferisce&d"),
              QObject::tr("&pLa forza oscura proveniente dal tocco di %1 ferisce %2&d"));
}

void CommandSpell::spellDamageTarget(Character *ch, Player *player, Entity *target, QMUD::SpellType spell,
                                     QMUD::DamageType damageType, int manaPoints, int damage, int lag,
                                     QString messageCh, QString messageTarget, QString messageOther)
{
    // Controllo se posso lanciare questo incantesimo
    if (!ch->canUseSpell(spell))
    {
        if (player)
            player->sendLine(QObject::tr("Non hai le capacita' per lanciare questo incantesimo"));
        return;
    }

    // Cerco il target
    if (!target)
    {
        target = ch->targetCharacter(true);

        if (!target)
        {
            if (player)
                player->sendLine(QObject::tr("Su chi?"));
            return;
        }
    }

    // Controllo se il target e' un character
    if (target->entityType() != Entity::EntityType::CHARACTER)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return;
    }

    // Controllo se posso attaccare il target
    Character* targetCh = static_cast<Character*>(target);

    QString message;

    if (!ch->canAttack(targetCh, message))
    {
        if (player)
            player->sendLine(message);
        return;
    }

    // Controllo se ho abbastanza mana
    if (ch->manaPointsCurrent() < manaPoints)
    {
        if (player)
            player->sendLine(QObject::tr("Non hai abbastanza mana"));
        return;
    }

    // Controllo se sono stunnato
    if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::STUNNED))
    {
        if (ch->intelligence() > ch->wisdom())
        {
            if (ch->tsIntelligence() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return;
            }
        }
        else
        {
            if (ch->tsWisdom() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return;
            }
        }
    }

    // Lancio l'incantesimo
    int damageDone = targetCh->calcDamage(damage, damageType);

    QString msgCh = QString(messageCh + "&R[%2]&d").arg(targetCh->name(ch)).arg(damageDone);
    QString msgTarget = QString(messageTarget + "&R[%2]&d").arg(ch->name(targetCh)).arg(damageDone);
    QString msgOther = QString(messageOther + "&R[%3]&d");

    if (player)
        player->sendLine(msgCh);

    if (targetCh->chType() == Character::ChType::CHTYPE_PC)
        static_cast<PcCharacter*>(targetCh)->player()->sendLine(msgTarget);

    auto characters = ch->room()->characters();

    for (Character* chInRoom : characters)
    {
        if (chInRoom == ch || chInRoom == targetCh)
            continue;

        if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
        {
            Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
            playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(targetCh->name(chInRoom)).arg(damageDone));
        }
    }

    targetCh->damage(damage, damageType, ch);

    ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);

    ch->setTarget(targetCh->id(), false);
    ch->setInCombat(true);

    targetCh->setTarget(ch->id(), false);
    targetCh->setInCombat(true);

    ch->useManaPoint(manaPoints);
}

void CommandSpell::spellDamageZone(Character *ch, Player *player, QMUD::SpellType spell, QMUD::DamageType damageType,
                             int manaPoints, int damage, int lag,
                             QString messageCh, QString messageTarget, QString messageOther)
{
    // Controllo se posso lanciare questo incantesimo
    if (!ch->canUseSpell(spell))
    {
        if (player)
            player->sendLine(QObject::tr("Non hai le capacita' per lanciare questo incantesimo"));
        return;
    }

    // Cerco i target
    auto characters = ch->room()->characters();
    QVector<Character*> charactersToAttack;

    for (Character* chInRoom : characters)
    {
        QString message;

        if (chInRoom == ch || !ch->canAttack(chInRoom, message))
            continue;

        charactersToAttack.push_back(chInRoom);
    }

    if (charactersToAttack.isEmpty())
    {
        if (player)
            player->sendLine(QObject::tr("Non c'e' nessuno da attaccare nella stanza"));
        return;
    }

    // Controllo se ho abbastanza mana
    if (ch->manaPointsCurrent() < manaPoints)
    {
        if (player)
            player->sendLine(QObject::tr("Non hai abbastanza mana"));
        return;
    }

    // Controllo se sono stunnato
    if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::STUNNED))
    {
        if (ch->intelligence() > ch->wisdom())
        {
            if (ch->tsIntelligence() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return;
            }
        }
        else
        {
            if (ch->tsWisdom() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return;
            }
        }
    }

    // Lancio l'incantesimo
    for (auto targetCh : charactersToAttack)
    {
        int damageDone = targetCh->calcDamage(damage, damageType);
        QString msgCh = QString(messageCh + "&R[%2]&d").arg(targetCh->name(ch)).arg(damageDone);
        QString msgTarget = QString(messageTarget + "&R[%2]&d").arg(ch->name(targetCh)).arg(damageDone);
        QString msgOther = QString(messageOther + "&R[%3]&d");

        if (player)
            player->sendLine(msgCh);

        if (targetCh->chType() == Character::ChType::CHTYPE_PC)
            static_cast<PcCharacter*>(targetCh)->player()->sendLine(msgTarget);

        for (Character* chInRoom : characters)
        {
            if (chInRoom == ch || chInRoom == targetCh)
                continue;

            if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
                static_cast<PcCharacter*>(chInRoom)->player()->sendLine(msgOther.arg(ch->name(chInRoom)).arg(targetCh->name(chInRoom)).arg(damageDone));
        }

        targetCh->damage(damage, damageType, ch);

        ch->setTarget(targetCh->id(), false);
        ch->setInCombat(true);

        targetCh->setTarget(ch->id(), false);
        targetCh->setInCombat(true);
    }

    ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
    ch->useManaPoint(manaPoints);
}

void CommandSpell::spellCureTarget(Character *ch, Player *player, Entity *target, QMUD::SpellType spell,
                                   int manaPoints, int cure, int lag,
                                   QString messageCh, QString messageTarget, QString messageOther,
                                   QString messageChMe, QString messageOtherMe)
{
    // Controllo se posso lanciare l'incantesimo
    if (!ch->canUseSpell(spell))
    {
        if (player)
            player->sendLine(QObject::tr("Non hai le capacita' per lanciare questo incantesimo"));
        return;
    }

    // Cerco il target
    if (!target)
    {
        target = ch;
    }

    // Controllo se il target e' un character
    if (target->entityType() != Entity::EntityType::CHARACTER)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return;
    }

    // Controllo se ho abbastanza mana
    if (ch->manaPointsCurrent() < manaPoints)
    {
        if (player)
            player->sendLine(QObject::tr("Non hai abbastanza mana"));
        return;
    }

    // Controllo se sono stunnato
    if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::STUNNED))
    {
        if (ch->intelligence() > ch->wisdom())
        {
            if (ch->tsIntelligence() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return;
            }
        }
        else
        {
            if (ch->tsWisdom() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return;
            }
        }
    }

    // Lancio l'incantesimo
    Character* targetCh = static_cast<Character*>(target);
    QString msgCh;
    QString msgTarget;
    QString msgOther;

    if (targetCh != ch)
    {
        msgCh = QString(messageCh + " &G[%2]&d").arg(targetCh->name(ch)).arg(cure);
        msgTarget = QString(messageTarget + " &G[%2]&d").arg(ch->name(targetCh)).arg(cure);
        msgOther = QString(messageOther + " &G[%3]&d");
    }
    else
    {
        msgCh = QString(messageChMe + " &G[%1]&d").arg(cure);
        msgTarget = msgCh;
        msgOther = QString(messageOtherMe + " &G[%2]&d");
    }

    if (player)
        player->sendLine(msgCh);

    if (ch != targetCh && targetCh->chType() == Character::ChType::CHTYPE_PC)
        static_cast<PcCharacter*>(targetCh)->player()->sendLine(msgTarget);

    auto characters = ch->room()->characters();

    for (Character* chInRoom : characters)
    {
        if (chInRoom == ch || chInRoom == targetCh)
            continue;

        if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
        {
            Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

            if (targetCh != ch)
                playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(targetCh->name(chInRoom)).arg(cure));
            else
                playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(cure));
        }
    }

    ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
    ch->useManaPoint(manaPoints);

    targetCh->healingHitPoints(cure, ch);
}

bool CommandSpell::spellAddStatusTarget(Character *ch, Player *player, Entity *target,
                                  QMUD::SpellType spell, QMUD::TemporaryStatusType status,
                                  int manaPoints, int turns, int lag,
                                  QString messageCh, QString messageTarget, QString messageOther,
                                  QString messageChMe, QString messageOtherMe)
{
    // Controllo se posso lanciare l'incantesimo
    if (!ch->canUseSpell(spell))
    {
        if (player)
            player->sendLine(QObject::tr("Non hai le capacita' per lanciare questo incantesimo"));
        return false;
    }

    // Cerco il target
    if (!target)
    {
        target = ch;
    }

    // Controllo se il target e' un character
    if (target->entityType() != Entity::EntityType::CHARACTER)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return false;
    }

    // Controllo se sono in combattimento
    if (ch->isInCombat())
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo mentre stai combattendo"));
        return false;
    }

    // Controllo se ho abbastanza mana
    if (ch->manaPointsCurrent() < manaPoints)
    {
        if (player)
            player->sendLine(QObject::tr("Non hai abbastanza mana"));
        return false;
    }

    // Controllo se sono stunnato
    if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::STUNNED))
    {
        if (ch->intelligence() > ch->wisdom())
        {
            if (ch->tsIntelligence() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return false;
            }
        }
        else
        {
            if (ch->tsWisdom() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return false;
            }
        }
    }

    // Lancio l'incantesimo
    Character* targetCh = static_cast<Character*>(target);
    QString msgCh;
    QString msgTarget;
    QString msgOther;

    if (targetCh != ch)
    {
        msgCh = messageCh.arg(targetCh->name(ch));
        msgTarget = messageTarget.arg(ch->name(targetCh));
        msgOther = messageOther;
    }
    else
    {
        msgCh = messageChMe;
        msgTarget = msgCh;
        msgOther = messageOtherMe;
    }

    if (player)
        player->sendLine(msgCh);

    if (ch != targetCh && targetCh->chType() == Character::ChType::CHTYPE_PC)
        static_cast<PcCharacter*>(targetCh)->player()->sendLine(msgTarget);

    auto characters = ch->room()->characters();

    for (Character* chInRoom : characters)
    {
        if (chInRoom == ch || chInRoom == targetCh)
            continue;

        if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
        {
            Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

            if (targetCh != ch)
                playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(targetCh->name(chInRoom)));
            else
                playerOther->sendLine(msgOther.arg(ch->name(chInRoom)));
        }
    }

    ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
    ch->useManaPoint(manaPoints);

    targetCh->setTemporaryStatus(status, turns, true);

    return true;
}

bool CommandSpell::spellAddStatusShield(Character *ch, Player *player, Entity *target, QMUD::SpellType spell, QMUD::TemporaryStatusType status, int manaPoints, int turns, int lag, QString messageChMe, QString messageOtherMe)
{
    // Controllo se posso lanciare l'incantesimo
    if (!ch->canUseSpell(spell))
    {
        if (player)
            player->sendLine(QObject::tr("Non hai le capacita' per lanciare questo incantesimo"));
        return false;
    }

    // Cerco il target
    if (!target)
    {
        target = ch;
    }

    // Controllo se il target e' un character
    if (target->entityType() != Entity::EntityType::CHARACTER)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo su %1").arg(target->name(Q_NULLPTR)));
        return false;
    }

    // Controllo se sono in combattimento
    if (ch->isInCombat())
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi lanciare questo incantesimo mentre stai combattendo"));
        return false;
    }

    // Controllo se ho abbastanza mana
    if (ch->manaPointsCurrent() < manaPoints)
    {
        if (player)
            player->sendLine(QObject::tr("Non hai abbastanza mana"));
        return false;
    }

    // Controllo se sono stunnato
    if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::STUNNED))
    {
        if (ch->intelligence() > ch->wisdom())
        {
            if (ch->tsIntelligence() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return false;
            }
        }
        else
        {
            if (ch->tsWisdom() < 20)
            {
                if (player)
                    player->sendLine(QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED);

                ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
                return false;
            }
        }
    }

    // Rimuovo gli altri scudi tranne nel caso siano dovuti ad oggetti (durata infinita)
    // Nel caso di un mago monoclasse, l'unico che casa blade shield, posso usare blade shield assieme
    // agli altri scudi
    int turnsOtherShield;
    bool isMage = ch->chClassType() == QMUD::ClassType::MAGE;

    if (status != QMUD::TemporaryStatusType::ICE_SHIELD &&
            ch->hasTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD, turnsOtherShield) &&
            turnsOtherShield != QMUD_GLOBAL_STATUS_DURATION_INFINITE)
        if ((status == QMUD::TemporaryStatusType::BLADES_SHIELD && !isMage) ||
             status != QMUD::TemporaryStatusType::BLADES_SHIELD)
            ch->removeTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD);

    if (status != QMUD::TemporaryStatusType::DIVINE_SHIELD &&
            ch->hasTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD, turnsOtherShield) &&
            turnsOtherShield != QMUD_GLOBAL_STATUS_DURATION_INFINITE)
        if ((status == QMUD::TemporaryStatusType::BLADES_SHIELD && !isMage) ||
             status != QMUD::TemporaryStatusType::BLADES_SHIELD)
            ch->removeTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD);

    if (status != QMUD::TemporaryStatusType::FIRE_SHIELD &&
            ch->hasTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD, turnsOtherShield) &&
            turnsOtherShield != QMUD_GLOBAL_STATUS_DURATION_INFINITE)
        if ((status == QMUD::TemporaryStatusType::BLADES_SHIELD && !isMage) ||
             status != QMUD::TemporaryStatusType::BLADES_SHIELD)
            ch->removeTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD);

    if (status != QMUD::TemporaryStatusType::BLADES_SHIELD &&
            ch->hasTemporaryStatus(QMUD::TemporaryStatusType::BLADES_SHIELD, turnsOtherShield) &&
            turnsOtherShield != QMUD_GLOBAL_STATUS_DURATION_INFINITE &&
            !isMage)
        ch->removeTemporaryStatus(QMUD::TemporaryStatusType::BLADES_SHIELD);

    // Lancio l'incantesimo
    Character* targetCh = static_cast<Character*>(target);
    QString msgCh = messageChMe;
    QString msgTarget = msgCh;
    QString msgOther = messageOtherMe;

    if (player)
        player->sendLine(msgCh);

    if (ch != targetCh && targetCh->chType() == Character::ChType::CHTYPE_PC)
        static_cast<PcCharacter*>(targetCh)->player()->sendLine(msgTarget);

    auto characters = ch->room()->characters();

    for (Character* chInRoom : characters)
    {
        if (chInRoom == ch || chInRoom == targetCh)
            continue;

        if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
        {
            Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

            if (targetCh != ch)
                playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(targetCh->name(chInRoom)));
            else
                playerOther->sendLine(msgOther.arg(ch->name(chInRoom)));
        }
    }

    ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, lag, false);
    ch->useManaPoint(manaPoints);

    targetCh->setTemporaryStatus(status, turns, true);

    return true;
}
