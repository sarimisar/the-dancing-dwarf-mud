/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclasswarrior.h"
#include "character.h"
#include "../global.h"
#include "command.h"

using namespace CORE;

CharacterClassWarrior::CharacterClassWarrior() :
    CharacterClass(QMUD::ClassType::WARRIOR)
{

}

CharacterClassWarrior::~CharacterClassWarrior()
{

}

QString CharacterClassWarrior::toReadableString() const
{
    return "Guerriero";
}

int CharacterClassWarrior::hitDice() const
{
    return 10;
}

int CharacterClassWarrior::manaDice() const
{
    return 0;
}

qint64 CharacterClassWarrior::experienceToLevelUp(int level) const
{
    return QMUD::experienceByLevelBase(level);
}

void CharacterClassWarrior::AICombatAction(Character *ch, Character *target) const
{
    Q_UNUSED(ch)
    Q_UNUSED(target)

    bool targetIsCaster = QMUD::classTypeIsCaster(target->chClass()->classType());

    // Controllo se posso caricare
    if (targetIsCaster)
    {
        if (canUseAbility(ch->level(), QMUD::AbilityType::CHARGE))
        {
            Command::parse(QMUD::abilityToReadableString(QMUD::AbilityType::CHARGE), ch)->execute();
            return;
        }
    }
    else if (!targetIsCaster)
    {
        bool useKick = QMUD::rollDice(1, 20) > 5;

        if (useKick && canUseAbility(ch->level(), QMUD::AbilityType::KICK))
        {
            Command::parse(QMUD::abilityToReadableString(QMUD::AbilityType::KICK), ch)->execute();
            return;
        }
        else if (canUseAbility(ch->level(), QMUD::AbilityType::CHARGE))
        {
            Command::parse(QMUD::abilityToReadableString(QMUD::AbilityType::CHARGE), ch)->execute();
            return;
        }
    }
}

void CharacterClassWarrior::AIIdleAction(Character *ch) const
{
    Q_UNUSED(ch)
}

QVector<QMUD::ItemClassType> CharacterClassWarrior::itemClasses() const
{
    static QVector<QMUD::ItemClassType> c;

    if (c.isEmpty())
    {
        c.resize(2);
        c[0] = QMUD::ItemClassType::DPS_STR;
        c[1] = QMUD::ItemClassType::TANK;
        c.squeeze();
    }

    return c;
}

int CharacterClassWarrior::maxNumberOfAttacks(int level) const
{
    Q_UNUSED(level)

    return 4;
}

bool CharacterClassWarrior::canUseAbility(int level, QMUD::AbilityType ability) const
{
    if (ability == QMUD::AbilityType::CHARGE)
        return true;
    else if (ability == QMUD::AbilityType::KICK)
        return level >= 5;
    else if (ability == QMUD::AbilityType::PARRY)
        return level >= 25;
    else
        return false;
}

bool CharacterClassWarrior::canUseSpell(int level, QMUD::SpellType spell) const
{
    Q_UNUSED(spell)
    Q_UNUSED(level)

    return false;
}

int CharacterClassWarrior::hitBonus(int level) const
{
    Q_UNUSED(level)

    return level * 2;
}

int CharacterClassWarrior::damageBonus(int level) const
{
    Q_UNUSED(level)

    return level;
}
