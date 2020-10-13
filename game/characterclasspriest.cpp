/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclasspriest.h"
#include "character.h"
#include "../global.h"
#include "command.h"

using namespace CORE;

CharacterClassPriest::CharacterClassPriest() :
    CharacterClass(QMUD::ClassType::PRIEST)
{

}

CharacterClassPriest::~CharacterClassPriest()
{

}

QString CharacterClassPriest::toReadableString() const
{
    return "Chierico";
}

int CharacterClassPriest::hitDice() const
{
    return 6;
}

int CharacterClassPriest::manaDice() const
{
    return 8;
}

qint64 CharacterClassPriest::experienceToLevelUp(int level) const
{
    return QMUD::experienceByLevelBase(level);
}

int CharacterClassPriest::maxNumberOfAttacks(int level) const
{
    Q_UNUSED(level)

    return 1;
}

void CharacterClassPriest::AICombatAction(Character *ch, Character *target) const
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
    else if (canUseSpell(ch->level(), QMUD::SpellType::INFLICTING_WOUNDS))
    {
        Command::parse(QObject::tr("formulo '%1' %2").arg(QMUD::spellToReadableString(QMUD::SpellType::INFLICTING_WOUNDS)).arg(target->name(Q_NULLPTR)), ch)->execute();
    }
}

void CharacterClassPriest::AIIdleAction(Character *ch) const
{
    if (ch->hitPointsCurrent() < ch->hitPointsCurrentMaximum() &&
            canUseSpell(ch->level(), QMUD::SpellType::CURE))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::CURE)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::SANCTIFY) &&
            !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::SANCTIFY))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::SANCTIFY)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::DIVINE_SHIELD) &&
            !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::DIVINE_SHIELD)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::SEE_INVISIBILITY) &&
            !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::SEE_INVISIBILITY))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::SEE_INVISIBILITY)), ch)->execute();
    }
}

void CharacterClassPriest::AIBuff(Character *ch) const
{
    if (canUseSpell(ch->level(), QMUD::SpellType::DIVINE_SHIELD))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::SANCTIFY))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::SANCTIFY, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::SEE_INVISIBILITY))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::SEE_INVISIBILITY, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);
}

QVector<QMUD::ItemClassType> CharacterClassPriest::itemClasses() const
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

bool CharacterClassPriest::canUseAbility(int level, QMUD::AbilityType ability) const
{
    Q_UNUSED(level)
    Q_UNUSED(ability)

    return false;
}

bool CharacterClassPriest::canUseSpell(int level, QMUD::SpellType spell) const
{
    Q_UNUSED(level)

    if (spell == QMUD::SpellType::CURE && level >= 1)
        return true;
    else if (spell == QMUD::SpellType::INFLICTING_WOUNDS && level >= 1)
        return true;
    else if (spell == QMUD::SpellType::SANCTIFY && level >= 40)
        return true;
    else if (spell == QMUD::SpellType::DIVINE_SHIELD && level >= 60)
        return true;
    else if (spell == QMUD::SpellType::INVISIBILITY && level >= 35)
        return true;
    else if (spell == QMUD::SpellType::SEE_INVISIBILITY && level >= 35)
        return true;
    else
        return false;
}
