/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemfood.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"

#include <QJsonArray>

using namespace CORE;

ItemFood::ItemFood(Entity *parent) :
    Item(QMUD::ItemType::FOOD, parent),
    m_iHpGainBonus(0),
    m_iApGainBonus(0)
{

}

ItemFood::~ItemFood()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemFood", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *CORE::ItemFood::loadItem(const QJsonObject &obj, Entity *parent)
{
    // HpGainBonus
    int tmpHpGainBonus;
    if (!QMUD::fileJsonRead(obj, "hpGainBonus", tmpHpGainBonus, true, 0))
        return Q_NULLPTR;

    // ApGainBonus
    int tmpApGainBonus;
    if (!QMUD::fileJsonRead(obj, "apGainBonus", tmpApGainBonus, true, 0))
        return Q_NULLPTR;

    ItemFood* item = new ItemFood(Q_NULLPTR);
    item->setParent(parent);
    item->m_iHpGainBonus = tmpHpGainBonus;
    item->m_iApGainBonus = tmpApGainBonus;

    return item;
}

void ItemFood::writeJson(QJsonObject &obj)
{
    Item::writeJson(obj);

    if (!IsModify())
        return;

    obj["hpGainBonus"] = m_iHpGainBonus;
    obj["apGainBonus"] = m_iApGainBonus;
}

int ItemFood::hpGainBonus() const
{
    return m_iHpGainBonus;
}

int ItemFood::apGainBonus() const
{
    return m_iApGainBonus;
}
