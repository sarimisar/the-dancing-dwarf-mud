/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemdrink.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"

#include <QJsonArray>

using namespace CORE;

ItemDrink::ItemDrink(Entity *parent) :
    Item(QMUD::ItemType::DRINK, parent),
    m_iMpGainBonus(0)
{

}

ItemDrink::~ItemDrink()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemDrink", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *CORE::ItemDrink::loadItem(const QJsonObject &obj, Entity *parent)
{
    // MpGainBonus
    int tmpMpGainBonus;
    if (!QMUD::fileJsonRead(obj, "mpGainBonus", tmpMpGainBonus, true, 0))
        return Q_NULLPTR;

    ItemDrink* item = new ItemDrink(Q_NULLPTR);
    item->setParent(parent);
    item->m_iMpGainBonus = tmpMpGainBonus;

    return item;
}

void ItemDrink::writeJson(QJsonObject &obj)
{
    Item::writeJson(obj);

    if (!IsModify())
        return;

    obj["mpGainBonus"] = m_iMpGainBonus;
}

int ItemDrink::mpGainBonus() const
{
    return m_iMpGainBonus;
}
