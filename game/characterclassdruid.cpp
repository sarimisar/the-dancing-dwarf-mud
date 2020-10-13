/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclassdruid.h"
#include "character.h"
#include "../global.h"
#include "command.h"

using namespace CORE;

CharacterClassDruid::CharacterClassDruid() :
    CharacterClass(QMUD::ClassType::DRUID)
{

}

CharacterClassDruid::~CharacterClassDruid()
{

}

QString CharacterClassDruid::toReadableString() const
{
    return "Druido";
}

int CharacterClassDruid::hitDice() const
{
    return 6;
}

int CharacterClassDruid::manaDice() const
{
    return 8;
}

qint64 CharacterClassDruid::experienceToLevelUp(int level) const
{
    return QMUD::experienceByLevelBase(level);
}

void CharacterClassDruid::AICombatAction(Character *ch, Character *target) const
{
    if (ch->hitPointsCurrent() < ch->hitPointsCurrentMaximum() && canUseSpell(ch->level(), QMUD::SpellType::CURE) && QMUD::rollDice(1, 5) == 1)
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::CURE)), ch)->execute();
    }
    else if (ch->hitPointsCurrent() < ch->hitPointsCurrentMaximum() / 2 && canUseSpell(ch->level(), QMUD::SpellType::CURE) && QMUD::rollDice(1, 5) == 1)
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::CURE)), ch)->execute();
    }
    else if (ch->hitPointsCurrent() < ch->hitPointsCurrentMaximum() / 5 && canUseSpell(ch->level(), QMUD::SpellType::CURE) && QMUD::rollDice(1, 4) == 1)
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::CURE)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::FIERY_ARROW) && QMUD::rollDice(1, 2) == 1)
    {
        Command::parse(QObject::tr("formulo '%1' %2").arg(QMUD::spellToReadableString(QMUD::SpellType::FIERY_ARROW)).arg(target->name(Q_NULLPTR)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::INFLICTING_WOUNDS))
    {
        Command::parse(QObject::tr("formulo '%1' %2").arg(QMUD::spellToReadableString(QMUD::SpellType::INFLICTING_WOUNDS)).arg(target->name(Q_NULLPTR)), ch)->execute();
    }
}

void CharacterClassDruid::AIIdleAction(Character *ch) const
{
    if (ch->hitPointsCurrent() < ch->hitPointsCurrentMaximum() &&
            canUseSpell(ch->level(), QMUD::SpellType::CURE))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::CURE)), ch)->execute();
    }
}

void CharacterClassDruid::AIBuff(Character *ch) const
{
    if (canUseSpell(ch->level(), QMUD::SpellType::ICE_SHIELD) && QMUD::rollDice(1, 2) == 1)
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);
    else if (canUseSpell(ch->level(), QMUD::SpellType::FIRE_SHIELD))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::SEE_INVISIBILITY))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::SEE_INVISIBILITY, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::FLY))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::FLY, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);
}

QVector<QMUD::ItemClassType> CharacterClassDruid::itemClasses() const
{
    static QVector<QMUD::ItemClassType> c;

    if (c.isEmpty())
    {
        c.resize(1);
        c[0] = QMUD::ItemClassType::CASTER_WIS;
        c.squeeze();
    }

    return c;
}

int CharacterClassDruid::maxNumberOfAttacks(int level) const
{
    Q_UNUSED(level)

    return 1;
}

bool CharacterClassDruid::canUseAbility(int level, QMUD::AbilityType ability) const
{
    Q_UNUSED(level)
    Q_UNUSED(ability)

    return false;
}

bool CharacterClassDruid::canUseSpell(int level, QMUD::SpellType spell) const
{
    Q_UNUSED(level)

    if (spell == QMUD::SpellType::CURE && level >= 1)
        return true;
    else if (spell == QMUD::SpellType::INFLICTING_WOUNDS && level >= 1)
        return true;
    else if (spell == QMUD::SpellType::INVISIBILITY && level >= 20)
        return true;
    else if (spell == QMUD::SpellType::SEE_INVISIBILITY && level >= 20)
        return true;
    else if (spell == QMUD::SpellType::BURNING_HANDS && level >= 10)
        return true;
    else if (spell == QMUD::SpellType::FLY && level >= 20)
        return true;
    else if (spell == QMUD::SpellType::ICE_STORM && level >= 40)
        return true;
    else if (spell == QMUD::SpellType::FIRE_SHIELD && level >= 50)
        return true;
    else if (spell == QMUD::SpellType::ICE_SHIELD && level >= 60)
        return true;
    else if (spell == QMUD::SpellType::FIERY_ARROW && level >= 30)
        return true;
    else
        return false;
}
