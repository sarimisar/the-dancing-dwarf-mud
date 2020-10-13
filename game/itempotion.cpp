/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itempotion.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"

#include <QJsonArray>

using namespace CORE;

ItemPotion::ItemPotion(Entity *parent) :
    Item(QMUD::ItemType::POTION, parent),
    m_iDurationSeconds(60),
    m_iHitPoints(0),
    m_iManaPoints(0),
    m_iActionPoints(0)
{

}

ItemPotion::~ItemPotion()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemPotion", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *CORE::ItemPotion::loadItem(const QJsonObject &obj, Entity *parent)
{
    // DurationSeconds
    int tmpDurationSeconds;
    if (!QMUD::fileJsonRead(obj, "durationSeconds", tmpDurationSeconds, true, 0))
        return Q_NULLPTR;

    // HitPoins
    int tmpHP;
    if (!QMUD::fileJsonRead(obj, "hitPoints", tmpHP, true, 0))
        return Q_NULLPTR;

    // ManaPoins
    int tmpMP;
    if (!QMUD::fileJsonRead(obj, "manaPoints", tmpMP, true, 0))
        return Q_NULLPTR;

    // ActionPoins
    int tmpAP;
    if (!QMUD::fileJsonRead(obj, "actionPoints", tmpAP, true, 0))
        return Q_NULLPTR;

    // Base Temporary Status
    QMUD::ItemBaseStatus baseStatus;
    auto itBaseStatus = obj.find("baseStatus");
    if (itBaseStatus != obj.end() && itBaseStatus->isObject())
    {
        if (!baseStatus.load(itBaseStatus->toObject()))
        {
            GLOBAL_CRITICAL(tr("Chiave 'baseStatus' non valida"));
            return Q_NULLPTR;
        }
    }

    if (tmpDurationSeconds <= 0 && !baseStatus.temporaryStatus().isEmpty())
    {
        GLOBAL_CRITICAL(tr("Chiave 'baseStatus' necessita di durationSeconds valida"));
        return Q_NULLPTR;
    }

    ItemPotion* item = new ItemPotion(Q_NULLPTR);
    item->setParent(parent);
    item->m_iDurationSeconds = tmpDurationSeconds;
    item->m_iHitPoints = tmpHP;
    item->m_iManaPoints = tmpMP;
    item->m_iActionPoints = tmpAP;
    item->m_baseTemporaryStatus = baseStatus;

    return item;
}

void ItemPotion::writeJson(QJsonObject &obj)
{
    Item::writeJson(obj);

    if (!IsModify())
        return;

    QJsonObject objItemBaseStatus;
    m_baseTemporaryStatus.write(objItemBaseStatus);
    obj["baseStatus"] = objItemBaseStatus;
}

int ItemPotion::durationSeconds() const
{
    return m_iDurationSeconds;
}

int ItemPotion::hitPoints() const
{
    return m_iHitPoints;
}

int ItemPotion::manaPoints() const
{
    return m_iManaPoints;
}

int ItemPotion::actionPoints() const
{
    return m_iActionPoints;
}

const QMUD::ItemBaseStatus &ItemPotion::baseTemporaryStatus() const
{
    return m_baseTemporaryStatus;
}
