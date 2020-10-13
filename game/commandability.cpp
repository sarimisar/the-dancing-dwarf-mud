/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandability.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "characterclass.h"
#include "itemweapon.h"

using namespace CORE;

CommandAbility::CommandAbility(QMUD::AbilityType abilityType, ActionType action) :
    Command(),
    m_eAbilityType(abilityType),
    m_eActionType(action)
{

}

CommandAbility::~CommandAbility()
{

}

bool CommandAbility::isOf(QString cmd, QMUD::AbilityType& abilityType, ActionType& action)
{
    if (Command::isOf(QObject::tr("caricare"), cmd) ||
            Command::isOf(QObject::tr("carico"), cmd))
    {
        abilityType = QMUD::AbilityType::CHARGE;
        action = ActionType::ABILITY;
        return true;
    }
    else if (Command::isOf(QObject::tr("calciare"), cmd) ||
            Command::isOf(QObject::tr("calcio"), cmd))
    {
        abilityType = QMUD::AbilityType::KICK;
        action = ActionType::ABILITY;
        return true;
    }
    else if (Command::isOf(QObject::tr("pugnalare"), cmd) ||
            Command::isOf(QObject::tr("pugnalo"), cmd))
    {
        abilityType = QMUD::AbilityType::BACKSTAB;
        action = ActionType::ABILITY;
        return true;
    }
    else if (Command::isOf(QObject::tr("abilita"), cmd))
    {
        abilityType = QMUD::AbilityType::UNKNOWN;
        action = ActionType::ABILITY_LIST;
        return true;
    }
    else if (Command::isOf(QObject::tr("parata"), cmd) ||
             Command::isOf(QObject::tr("parare"), cmd))
    {
        abilityType = QMUD::AbilityType::PARRY;
        action = ActionType::ABILITY;
        return true;
    }
    else if (Command::isOf(QObject::tr("schivata"), cmd) ||
             Command::isOf(QObject::tr("schivare"), cmd))
    {
        abilityType = QMUD::AbilityType::DODGE;
        action = ActionType::ABILITY;
        return true;
    }

    return false;
}

QStringList CommandAbility::commands()
{
    return QStringList() << QObject::tr("carica")
                         << QObject::tr("calcio")
                         << QObject::tr("pugnalo")
                         << QObject::tr("parata")
                         << QObject::tr("schivata")
                         << QObject::tr("abilita");
}

void CommandAbility::executePc(Character* ch, Player* player)
{
    if (m_eActionType == ActionType::ABILITY)
    {
        if (!ch->canUseAbility(m_eAbilityType))
        {
            if (player)
                player->sendLine(QObject::tr("Non puoi farlo"));
            return;
        }

        Character* target = ch->targetCharacter(true);
        QString wrongTarget;

        if (!target && !args().isEmpty())
        {
            bool wrongUsage;
            auto targetEntity = findTargetInRoom(args()[0], wrongUsage, true);

            if (wrongUsage)
            {
                sendUsage(player);
                return;
            }

            if (!targetEntity)
            {
                if (player)
                    player->sendLine(QObject::tr("Non c'e' nessun %1 qui").arg(args()[0]));
                return;
            }

            if (targetEntity->entityType() != Entity::EntityType::CHARACTER)
            {
                wrongTarget = targetEntity->name(Q_NULLPTR);
            }
            else
            {
                target = static_cast<Character*>(targetEntity);
            }
        }

        if (m_eAbilityType == QMUD::AbilityType::CHARGE)
        {
            QString message;
            if (!ch->canMakeAction(message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            if (ch->actionPointsCurrent() < 25)
            {
                if (player)
                    player->sendLine(QObject::tr("Non hai abbastanza punti azione!"));
                return;
            }

            if (!wrongTarget.isEmpty())
            {
                if (player)
                    player->sendLine(QObject::tr("Non puoi caricare %1").arg(wrongTarget));
                return;
            }

            if (!target)
            {
                if (player)
                    player->sendLine(QObject::tr("Caricare chi?"));
                return;
            }

            if (!ch->canAttack(target, message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            QString msgCh;
            QString msgTarget;
            QString msgOther;

            int baseC = (ch->constitution()/3 + ch->strength()/2);
            int abilityLevel = ch->level();
            int damage = QMUD::rollDice(abilityLevel, 6) + baseC;
            int damageDone = target->calcDamage(damage, QMUD::DamageType::HANDS);

            msgCh = QObject::tr("&RCarichi %1 mandandolo a gambe levate! [%2]&d").arg(target->name(ch)).arg(damageDone);
            msgTarget = QObject::tr("&R%1 ti carica mandandoti a gambe levate! [%2]&d").arg(ch->name(ch)).arg(damageDone);
            msgOther = QObject::tr("&R%1 carica %2 mandandolo a gambe levate! [%3]&d");

            if (player)
                player->sendLine(msgCh);

            if (target->chType() == Character::ChType::CHTYPE_PC)
                static_cast<PcCharacter*>(target)->player()->sendLine(msgTarget);

            auto characters = ch->room()->characters();

            for (Character* chInRoom : characters)
            {
                if (chInRoom == ch || chInRoom == target)
                    continue;

                if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
                {
                    Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
                    playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(target->name(chInRoom)).arg(damageDone));
                }
            }

            ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 2, false);
            ch->setTarget(target->id(), false);
            ch->setInCombat(true);

            target->setTarget(ch->id(), false);
            target->setInCombat(true);

            ch->useActionPoint(25);

            target->setTemporaryStatus(QMUD::TemporaryStatusType::STUNNED, 2, false);
            target->damage(damage, QMUD::DamageType::HANDS, ch);
        }
        else if (m_eAbilityType == QMUD::AbilityType::KICK)
        {
            QString message;
            if (!ch->canMakeAction(message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            if (ch->actionPointsCurrent() < 50)
            {
                if (player)
                    player->sendLine(QObject::tr("Non hai abbastanza punti azione!"));
                return;
            }

            if (!wrongTarget.isEmpty())
            {
                if (player)
                    player->sendLine(QObject::tr("Non puoi calciare %1").arg(wrongTarget));
                return;
            }

            if (!target)
            {
                if (player)
                    player->sendLine(QObject::tr("Calciare chi?"));
                return;
            }

            if (!ch->canAttack(target, message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            QString msgCh;
            QString msgTarget;
            QString msgOther;

            int baseC = (ch->dexterity()/2 + ch->strength()/2);
            int abilityLevel = qMax(1, ch->level());
            int damage = QMUD::rollDice(abilityLevel, 6) + baseC;
            int damageDone = target->calcDamage(damage, QMUD::DamageType::HANDS);
            msgCh = QObject::tr("&RSferri un calcio ben piazzato a %1! [%2]&d").arg(target->name(ch)).arg(damageDone);
            msgTarget = QObject::tr("&R%1 ti sferra un calcio ben piazzato! [%2]&d").arg(ch->name(target)).arg(damageDone);
            msgOther = QObject::tr("&R%1 sferra un calcio ben piazzati a %2! [%3]&d");

            if (player)
                player->sendLine(msgCh);

            if (target->chType() == Character::ChType::CHTYPE_PC)
                static_cast<PcCharacter*>(target)->player()->sendLine(msgTarget);

            auto characters = ch->room()->characters();

            for (Character* chInRoom : characters)
            {
                if (chInRoom == ch || chInRoom == target)
                    continue;

                if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
                {
                    Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
                    playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(target->name(chInRoom)).arg(damageDone));
                }
            }

            ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 1, false);
            ch->setTarget(target->id(), false);
            ch->setInCombat(true);

            target->setTarget(ch->id(), false);
            target->setInCombat(true);

            ch->useActionPoint(50);

            target->damage(damage, QMUD::DamageType::HANDS, ch);
        }
        else if (m_eAbilityType == QMUD::AbilityType::BACKSTAB)
        {
            auto weapon = ch->equippedWeapon();

            if (!weapon || weapon->damageType() != QMUD::DamageType::PIERCING)
            {
                if (player)
                    player->sendLine(QObject::tr("Per pugnalare qualcuno devi impugnare un pugnale o una lancia"));
                return;
            }

            QString message;
            if (!ch->canMakeAction(message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            if (ch->actionPointsCurrent() < 10)
            {
                if (player)
                    player->sendLine(QObject::tr("Non hai abbastanza punti azione!"));
                return;
            }

            if (!wrongTarget.isEmpty())
            {
                if (player)
                    player->sendLine(QObject::tr("Non puoi pugnalare %1").arg(wrongTarget));
                return;
            }

            if (!target)
            {
                if (player)
                    player->sendLine(QObject::tr("Pugnalare chi?"));
                return;
            }

            if (!ch->canAttack(target, message))
            {
                if (player)
                    player->sendLine(message);
                return;
            }

            if (ch->isInCombat())
            {
                if (player)
                    player->sendLine(QObject::tr("Non puoi usare questa abilita in combattimento"));
                return;
            }

            QString msgCh;
            QString msgTarget;
            QString msgOther;

            bool actionIsOk = false;
            int weaponHit = ch->rollAttackHit();
            int abilityLevel = qMax(1, ch->level());
            int damage = (ch->rollAttackDamage() + ch->dexterity()) * abilityLevel / 4;
            int damageDone = target->calcDamage(damage, QMUD::DamageType::HANDS);

            if (weaponHit >= target->armorClass())
            {
                actionIsOk = true;
                msgCh = QObject::tr("&RPugnali alle spalle %1! [%2]&d").arg(target->name(ch)).arg(damageDone);
                msgTarget = QObject::tr("&R%1 ti pugnala alle spalle! [%2]&d").arg(ch->name(target)).arg(damageDone);
                msgOther = QObject::tr("&R%1 pugnala alle spalle %2! [%3]&d");
            }
            else
            {
                msgCh = QObject::tr("&R%1 schiva la tua pugnalata! [0]&d").arg(target->name(ch));
                msgTarget = QObject::tr("&RSchivi la pugnalata di %1! [0]&d").arg(ch->name(target));
                msgOther = QObject::tr("&R%2 schiva la pugnalata %1! [%3]&d");
            }

            if (player)
                player->sendLine(msgCh);

            if (target->chType() == Character::ChType::CHTYPE_PC)
                static_cast<PcCharacter*>(target)->player()->sendLine(msgTarget);

            auto characters = ch->room()->characters();

            for (Character* chInRoom : characters)
            {
                if (chInRoom == ch || chInRoom == target)
                    continue;

                if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
                {
                    Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
                    playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(target->name(chInRoom)).arg(damageDone));
                }
            }

            ch->setTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG, 2, false);
            ch->setTarget(target->id(), false);
            ch->setInCombat(true);

            target->setTarget(ch->id(), false);
            target->setInCombat(true);

            ch->useActionPoint(10);

            if (actionIsOk)
                target->damage(damage, QMUD::DamageType::HANDS, ch);
        }
        else if (m_eAbilityType == QMUD::AbilityType::PARRY)
        {
            if (player)
                player->sendLine(QObject::tr("Parata e' una abilita passiva"));
        }
        else if (m_eAbilityType == QMUD::AbilityType::DODGE)
        {
            if (player)
                player->sendLine(QObject::tr("Schivata e' una abilita passiva"));
        }
        else
            Q_ASSERT(false);
    }
    else if (m_eActionType == ActionType::ABILITY_LIST)
    {
        if (player)
        {
            QString spellList;

            for (int i = static_cast<int>(QMUD::AbilityType::UNKNOWN); i < static_cast<int>(QMUD::AbilityType::MAX_VALUE); ++i)
                if (ch->canUseAbility(static_cast<QMUD::AbilityType>(i)))
                    spellList += QMUD_GLOBAL_NEW_LINE + QString("  ") + QMUD::abilityToReadableString(static_cast<QMUD::AbilityType>(i));

            if (!spellList.isEmpty())
            {
                player->sendLine(QObject::tr("Conosci le seguenti abilita':"));
                player->sendLine(spellList);
            }
            else
            {
                player->sendLine(QObject::tr("Non conosci nessuna abilita'"));
            }
        }
    }
}

void CommandAbility::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandAbility::sendUsage(Player *player)
{
    if (player)
    {
        if (m_eAbilityType == QMUD::AbilityType::CHARGE)
            player->sendLine(QObject::tr("Uso: carica [<n>.][nome]"));
        else if (m_eAbilityType == QMUD::AbilityType::KICK)
            player->sendLine(QObject::tr("Uso: calcia [<n>.][nome]"));
        else if (m_eAbilityType == QMUD::AbilityType::BACKSTAB)
            player->sendLine(QObject::tr("Uso: pugnala [<n>.][nome]"));
        else if (m_eAbilityType == QMUD::AbilityType::PARRY)
            player->sendLine(QObject::tr("L'abilita' parata e' una abilita' passiva che permette di bloccare," QMUD_GLOBAL_NEW_LINE
                                         "tramite l'utilizzo di uno scudo, gli attacchi dell'avversario"));
        else if (m_eAbilityType == QMUD::AbilityType::DODGE)
            player->sendLine(QObject::tr("L'abilita' schivata e' una abilita' passiva che permette di eludere," QMUD_GLOBAL_NEW_LINE
                                         "gli attacchi dell'avversario"));
        else if (m_eActionType == ActionType::ABILITY_LIST)
            player->sendLine(QObject::tr("Mostra l'elenco delle abilita' disponibili"));
    }
}
