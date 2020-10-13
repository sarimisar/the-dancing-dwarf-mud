/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemtonic.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"

#include <QJsonArray>

using namespace CORE;

ItemTonic::ItemTonic(Entity *parent) :
    Item(QMUD::ItemType::TONIC, parent),
    m_iDurationSeconds(60)
{

}

ItemTonic::~ItemTonic()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemTonic", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *CORE::ItemTonic::loadItem(const QJsonObject &obj, Entity *parent)
{
    // DurationSeconds
    int tmpDurationSeconds;
    if (!QMUD::fileJsonRead(obj, "durationSeconds", tmpDurationSeconds, true, 0))
        return Q_NULLPTR;

    // Base bonus
    QMUD::ItemBaseBonus baseBonus;
    auto itBaseBonus = obj.find("baseBonus");
    if (itBaseBonus != obj.end() && itBaseBonus->isObject())
    {
        if (!baseBonus.load(itBaseBonus->toObject()))
        {
            GLOBAL_CRITICAL(tr("Chiave 'baseBonus' non valida"));
            return Q_NULLPTR;
        }
    }

    if (tmpDurationSeconds <= 0)
    {
        GLOBAL_CRITICAL(tr("Chiave 'baseBonus' necessita di durationSeconds valida"));
        return Q_NULLPTR;
    }

    ItemTonic* item = new ItemTonic(Q_NULLPTR);
    item->setParent(parent);
    item->m_iDurationSeconds = tmpDurationSeconds;
    item->m_itemBaseBonus = baseBonus;

    return item;
}

void ItemTonic::writeJson(QJsonObject &obj)
{
    Item::writeJson(obj);

    if (!IsModify())
        return;

    QJsonObject objBaseBonus;
    m_itemBaseBonus.write(objBaseBonus);
    obj["baseBonus"] = objBaseBonus;
}

int ItemTonic::durationSeconds() const
{
    return m_iDurationSeconds;
}

const QMUD::ItemBaseBonus &CORE::ItemTonic::baseBonus() const
{
    return m_itemBaseBonus;
}
