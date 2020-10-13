/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclassmage.h"
#include "character.h"
#include "command.h"

using namespace CORE;

CharacterClassMage::CharacterClassMage() :
    CharacterClass(QMUD::ClassType::MAGE)
{

}

CharacterClassMage::~CharacterClassMage()
{

}

QString CharacterClassMage::toReadableString() const
{
    return QObject::tr("Mago");
}

int CharacterClassMage::hitDice() const
{
    return 6;
}

int CharacterClassMage::manaDice() const
{
    return 8;
}

qint64 CharacterClassMage::experienceToLevelUp(int level) const
{
    return QMUD::experienceByLevelBase(level);
}

void CharacterClassMage::AICombatAction(Character *ch, Character *target) const
{
    if (canUseSpell(ch->level(), QMUD::SpellType::FIERY_ARROW) && QMUD::rollDice(1, 2) == 1)
    {
        Command::parse(QObject::tr("formulo '%1' %2").arg(QMUD::spellToReadableString(QMUD::SpellType::FIERY_ARROW)).arg(target->name(Q_NULLPTR)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::MAGIC_MISSILE))
    {
        Command::parse(QObject::tr("formulo '%1' %2").arg(QMUD::spellToReadableString(QMUD::SpellType::MAGIC_MISSILE)).arg(target->name(Q_NULLPTR)), ch)->execute();
    }
}

void CharacterClassMage::AIIdleAction(Character *ch) const
{
    if (canUseSpell(ch->level(), QMUD::SpellType::SHIELD) &&
            !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::SHIELD))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::SHIELD)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::ILLUSORY_IMAGES) &&
             !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1) &&
             !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2) &&
             !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3) &&
             !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4) &&
             !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::ILLUSORY_IMAGES)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::STRENGHT) &&
            !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::STRENGHT))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::STRENGHT)), ch)->execute();
    }
    else if (canUseSpell(ch->level(), QMUD::SpellType::SEE_INVISIBILITY) &&
            !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::SEE_INVISIBILITY))
    {
        Command::parse(QObject::tr("formulo '%1'").arg(QMUD::spellToReadableString(QMUD::SpellType::SEE_INVISIBILITY)), ch)->execute();
    }
}

void CharacterClassMage::AIBuff(Character *ch) const
{
    if (canUseSpell(ch->level(), QMUD::SpellType::ICE_SHIELD) && QMUD::rollDice(1, 2) == 1)
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);
    else if (canUseSpell(ch->level(), QMUD::SpellType::FIRE_SHIELD))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::BLADES_SHIELD))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::BLADES_SHIELD, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::SHIELD))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::SHIELD, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::STRENGHT))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::STRENGHT, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::ILLUSORY_IMAGES))
    {
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

        ch->setTemporaryStatus(imagesStatus, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

        ch->illusoryImagesResetHP();
    }

    if (canUseSpell(ch->level(), QMUD::SpellType::FLY))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::FLY, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);

    if (canUseSpell(ch->level(), QMUD::SpellType::SEE_INVISIBILITY))
        ch->setTemporaryStatus(QMUD::TemporaryStatusType::SEE_INVISIBILITY, QMUD_GLOBAL_STATUS_DURATION_INFINITE, false);
}

QVector<QMUD::ItemClassType> CharacterClassMage::itemClasses() const
{
    static QVector<QMUD::ItemClassType> c;

    if (c.isEmpty())
    {
        c.resize(1);
        c[0] = QMUD::ItemClassType::CASTER_INT;
        c.squeeze();
    }

    return c;
}

int CharacterClassMage::maxNumberOfAttacks(int level) const
{
    Q_UNUSED(level)

    return 1;
}

bool CharacterClassMage::canUseAbility(int level, QMUD::AbilityType ability) const
{
    Q_UNUSED(level)
    Q_UNUSED(ability)

    return false;
}

bool CharacterClassMage::canUseSpell(int level, QMUD::SpellType spell) const
{
    Q_UNUSED(level)

    if (spell == QMUD::SpellType::MAGIC_MISSILE && level >= 1)
        return true;
    else if (spell == QMUD::SpellType::SHIELD && level >= 1)
        return true;
    else if (spell == QMUD::SpellType::BURNING_HANDS && level >= 5)
        return true;
    else if (spell == QMUD::SpellType::STRENGHT && level >= 10)
        return true;
    else if (spell == QMUD::SpellType::ILLUSORY_IMAGES && level >= 15)
        return true;
    else if (spell == QMUD::SpellType::FIREBALL && level >= 40)
        return true;
    else if (spell == QMUD::SpellType::FLY && level >= 20)
        return true;
    else if (spell == QMUD::SpellType::INVISIBILITY && level >= 25)
        return true;
    else if (spell == QMUD::SpellType::SEE_INVISIBILITY && level >= 25)
        return true;
    else if (spell == QMUD::SpellType::FIRE_SHIELD && level >= 45)
        return true;
    else if (spell == QMUD::SpellType::FIERY_ARROW && level >= 30)
        return true;
    else if (spell == QMUD::SpellType::ICE_STORM && level >= 60)
        return true;
    else if (spell == QMUD::SpellType::ICE_SHIELD && level >= 65)
        return true;
    else if (spell == QMUD::SpellType::BLADES_SHIELD && level >= 95)
        return true;
    else
        return false;
}
