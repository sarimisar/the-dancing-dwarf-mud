/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemtotem.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"
#include "world.h"

#include <QJsonArray>

using namespace CORE;


ItemTotem::ItemTotem(Entity *parent) :
    Item(QMUD::ItemType::TOTEM, parent),
    m_iLevel(1),
    m_eType(TotemType::BEAR)
{

}

ItemTotem::~ItemTotem()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemTotem", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

ItemTotem *ItemTotem::create(int level, ItemTotem::TotemType type)
{
    Q_UNUSED(level)
    Q_UNUSED(type)

    return Q_NULLPTR;
}

int ItemTotem::level() const
{
    return m_iLevel;
}

ItemTotem::TotemType ItemTotem::type() const
{
    return m_eType;
}
