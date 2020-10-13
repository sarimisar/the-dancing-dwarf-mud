/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclass.h"
#include "characterclasswarrior.h"
#include "characterclassmage.h"
#include "characterclassbarbarian.h"
#include "characterclassdruid.h"
#include "characterclassmonk.h"
#include "characterclasspriest.h"
#include "characterclassrogue.h"

using namespace CORE;

CharacterClass::~CharacterClass()
{

}

QMUD::ClassType CharacterClass::classType() const
{
    return m_eClassType;
}

CharacterClass *CharacterClass::fromString(QString name)
{
    return fromType(QMUD::classTypeFromString(name));
}

CharacterClass *CharacterClass::fromType(QMUD::ClassType type)
{
    if (type == QMUD::ClassType::WARRIOR)
        return CharacterClassWarrior::instance();
    else if (type == QMUD::ClassType::MAGE)
        return CharacterClassMage::instance();
    else if (type == QMUD::ClassType::BARBARIAN)
        return CharacterClassBarbarian::instance();
    else if (type == QMUD::ClassType::DRUID)
        return CharacterClassDruid::instance();
    else if (type == QMUD::ClassType::MONK)
        return CharacterClassMonk::instance();
    else if (type == QMUD::ClassType::PRIEST)
        return CharacterClassPriest::instance();
    else if (type == QMUD::ClassType::ROGUE)
        return CharacterClassRogue::instance();

    return Q_NULLPTR;
}

QString CharacterClass::toString(QMUD::ClassType type)
{
    return QMUD::classTypeToString(type);
}


QString CharacterClass::toReadableString(QMUD::ClassType type)
{
    return QMUD::classTypeToReadableString(type);
}

void CharacterClass::AIBuff(Character *ch) const
{
    Q_UNUSED(ch)
}

CharacterClass::CharacterClass(QMUD::ClassType classType) :
    m_eClassType(classType)
{

}

int CharacterClass::hitBonus(int level) const
{
    Q_UNUSED(level)

    return 0;
}

int CharacterClass::damageBonus(int level) const
{
    Q_UNUSED(level)

    return 0;
}

int CharacterClass::handDamageMin(int level) const
{
    Q_UNUSED(level)

    return 1;
}

int CharacterClass::handDamageMax(int level) const
{
    return level;
}

