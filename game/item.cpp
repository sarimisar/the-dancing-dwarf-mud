/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "item.h"
#include "character.h"
#include "itemweapon.h"
#include "itemarmor.h"
#include "itemtrinket.h"
#include "itemfood.h"
#include "itemdrink.h"
#include "itempotion.h"
#include "itemresource.h"
#include "itemtonic.h"
#include "itemtotem.h"
#include "itemsymbol.h"
#include "world.h"
#include "../global.h"
#include "../globaldebug.h"

#include <QJsonArray>

using namespace CORE;

Item::Item(QMUD::ItemType type, Entity *parent) :
    Entity(EntityType::ITEM, parent),
    m_eItemType(type),
    m_iStaticId(QMUD::StaticIdInvalid),
    m_iMapStaticId(QMUD::StaticIdInvalid),
    m_eQuality(QMUD::ItemQualityType::JUNK),
    m_iCoinsValue(0),
    m_iAutoDestroyCounter(-1),
    m_bIsBind(false),
    m_iLevel(1)
{

}

Item::~Item()
{
    if (m_eItemType == QMUD::ItemType::BASE)
        GLOBAL_DEBUG_DESTRUCTOR("Item", "Id: " + QString::number(id()) +
                                " StaticId: " + QString::number(staticId()) +
                                " MapId: " + QString::number(mapStaticId()));
}

Item *Item::loadItem(const QJsonObject &obj, Entity *parent, QMUD::StaticIdType mapStaticId)
{
    bool loadFromPcCharacterFile = false;

    QString randomCode;

    // MapId
    // Se mapStaticId e' un id valido lo assegno (caricamento da Map)
    // altrimenti carico l'id dal JsonObject (caricamento da PcCharacter)
    QMUD::StaticIdType tmpMapId;

    if (mapStaticId != QMUD::StaticIdInvalid)
        tmpMapId = mapStaticId;
    else
    {
        // Provo a caricare il randomCode
        if (!QMUD::fileJsonRead(obj, "randomCode", randomCode, false, true))
            return Q_NULLPTR;

        if (randomCode.isEmpty())
        {
            int tmpInt;
            if (!QMUD::fileJsonRead(obj, "mapId", tmpInt))
                return Q_NULLPTR;

            tmpMapId = static_cast<QMUD::StaticIdType>(tmpInt);
        }
        else
        {
            tmpMapId = QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM;
        }

        loadFromPcCharacterFile = true;
    }

    // UUID
    QString tmpUuidStr;
    if (!QMUD::fileJsonRead(obj, "uuid", tmpUuidStr, false, true))
        return Q_NULLPTR;

    QUuid tmpUuid;
    if (tmpUuidStr.isEmpty())
        tmpUuid = QUuid::createUuid();
    else
    {
        tmpUuid = QUuid(tmpUuidStr);

        if (tmpUuid.isNull())
        {
            GLOBAL_CRITICAL(tr("Chiave 'uuid' non valida: %1").arg(tmpUuidStr));
            return Q_NULLPTR;
        }
    }

    // Id
    double tmpId;
    if (randomCode.isEmpty())
    {
        if (!QMUD::fileJsonRead(obj, "id", tmpId))
            return Q_NULLPTR;
    }
    else
    {
        tmpId = QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM;
    }

    // IsBind
    bool tmpIsBind;
    if (!QMUD::fileJsonRead(obj, "isBind", tmpIsBind, true, false))
        return Q_NULLPTR;

    bool loadFromMap = loadFromPcCharacterFile && randomCode.isEmpty() && !obj.contains("name");
    bool loadRandomCodeNotModified = loadFromPcCharacterFile && !randomCode.isEmpty() && !obj.contains("name");

    if (loadFromMap)
    {
        Q_ASSERT(!loadRandomCodeNotModified);

        auto map = QMUD::qmudWorld->map(tmpMapId);

        if (!map)
        {
            GLOBAL_CRITICAL(tr("Chiave 'mapId' non valida: %1").arg(tmpMapId));
            return Q_NULLPTR;
        }

        auto objs = map->mapJsonObjItemsById();
        auto itemJsonIt = objs.find(static_cast<QMUD::StaticIdType>(qRound64(tmpId)));

        if (itemJsonIt == objs.end())
        {
            GLOBAL_CRITICAL(tr("Chiave 'id' non valida: %1 (mapId: %2)").arg(tmpId).arg(tmpMapId));
            return Q_NULLPTR;
        }

        return loadItem(itemJsonIt.value(), parent, tmpMapId);
    }
    else
    {
        if (loadRandomCodeNotModified)
        {
            Q_ASSERT(!randomCode.isEmpty());

            Item* item = createFromRandomCode(randomCode, parent);

            item->m_bIsBind = tmpIsBind;
            item->m_uuid = tmpUuid;

            return item;
        }
        else
        {
            // Type
            QString tmpType;
            if (!QMUD::fileJsonRead(obj, "type", tmpType))
                return Q_NULLPTR;

            tmpType = tmpType.simplified();

            // Level
            int tmpLevel;
            if (!QMUD::fileJsonRead(obj, "level", tmpLevel))
                return Q_NULLPTR;

            if (tmpLevel < 1)
                tmpLevel = 1;

            // Quality
            QString tmpQualityStr;
            if (!QMUD::fileJsonRead(obj, "quality", tmpQualityStr, false, true, QMUD::itemQualityToString(QMUD::ItemQualityType::JUNK)))
                return Q_NULLPTR;

            QMUD::ItemQualityType quality = QMUD::itemQualityFromString(tmpQualityStr);

            // Name
            QString tmpName;
            if (!QMUD::fileJsonRead(obj, "name", tmpName))
                return Q_NULLPTR;

            // Description
            QStringList tmpDescription;
            if (!QMUD::fileJsonRead(obj, "description", tmpDescription, false, false, true))
                return Q_NULLPTR;

            Item *item = Q_NULLPTR;

            if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::WEAPON), Qt::CaseInsensitive) == 0)
                item = ItemWeapon::loadItem(obj, parent, tmpLevel, quality);
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::ARMOR), Qt::CaseInsensitive) == 0)
                item = ItemArmor::loadItem(obj, parent, tmpLevel, quality);
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::TRINKET), Qt::CaseInsensitive) == 0)
                item = ItemTrinket::loadItem(obj, parent, tmpLevel, quality);
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::FOOD), Qt::CaseInsensitive) == 0)
                item = ItemFood::loadItem(obj, parent);
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::DRINK), Qt::CaseInsensitive) == 0)
                item = ItemDrink::loadItem(obj, parent);
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::POTION), Qt::CaseInsensitive) == 0)
                item = ItemPotion::loadItem(obj, parent);
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::TONIC), Qt::CaseInsensitive) == 0)
                item = ItemTonic::loadItem(obj, parent);
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::TOTEM), Qt::CaseInsensitive) == 0)
            {
                item = new ItemTotem(Q_NULLPTR);
                item->setParent(parent);
            }
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::SYMBOL), Qt::CaseInsensitive) == 0)
            {
                item = new ItemSymbol(Q_NULLPTR);
                item->setParent(parent);
            }
            else if (tmpType.compare(QMUD::itemTypeToString(QMUD::ItemType::BASE), Qt::CaseInsensitive) == 0)
            {
                item = new Item(QMUD::ItemType::BASE, Q_NULLPTR);
                item->setParent(parent);
            }
            else
                return Q_NULLPTR;

            if (!item)
            {
                GLOBAL_CRITICAL(tr("Oggetto %1 (id:%2) non valido").arg(tmpName).arg(tmpId));
                return Q_NULLPTR;
            }

            item->m_iStaticId = static_cast<QMUD::StaticIdType>(qRound64(tmpId));
            item->m_iLevel = tmpLevel;
            item->m_strName = tmpName;
            item->m_strDescription = tmpDescription.join(QMUD_GLOBAL_NEW_LINE);
            item->m_uuid = tmpUuid;
            item->m_bIsBind = tmpIsBind;
            item->m_iMapStaticId = tmpMapId;
            item->m_eQuality = quality;
            item->m_iCoinsValue = getDefaultCoinsValue(tmpLevel, quality);

            if (parent)
                QMUD::qmudWorld->addItem(item);
            return item;
        }
    }
}

QString Item::generateRandomCodeForCharacter(Character *ch, int level)
{
    QMUD::ItemQualityType quality = QMUD::ItemQualityType::JUNK;

    if (QMUD::rollDice(1, 2) == 1)
    {
        quality = QMUD::ItemQualityType::COMMON;

        if (QMUD::rollDice(1, 2) == 1)
        {
            quality = QMUD::ItemQualityType::UNCOMMON;

            if (QMUD::rollDice(1, 2) == 1)
            {
                quality = QMUD::ItemQualityType::VALUABLE;

                if (QMUD::rollDice(1, 2) == 1)
                {
                    quality = QMUD::ItemQualityType::RARE;

                    if (QMUD::rollDice(1, 2) == 1)
                    {
                        quality = QMUD::ItemQualityType::EPIC;
                    }
                }
            }
        }
    }

    return generateRandomCode(level, ch->chClass()->itemClasses()[0], quality);
}

QString Item::generateRandomCodeForCharacter(Character *ch, int level, QMUD::ItemQualityType quality)
{
    return generateRandomCode(level, ch->chClass()->itemClasses()[0], quality);
}

Item *Item::createFromRandomCode(QString randomCode, Entity *parent)
{
    if (ItemArmor::isValidRandomCode(randomCode))
        return ItemArmor::createFromRandomCode(randomCode, parent);
    else if (ItemWeapon::isValidRandomCode(randomCode))
        return ItemWeapon::createFromRandomCode(randomCode, parent);
    else if (ItemTrinket::isValidRandomCode(randomCode))
        return ItemTrinket::createFromRandomCode(randomCode, parent);
    else
        return Q_NULLPTR;
}

QString Item::generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality)
{
    int random = QMUD::rollDice(1, 3);

    if (random == 1)
        return ItemArmor::generateRandomCode(level, itemClass, quality);
    else if (random == 2)
        return ItemWeapon::generateRandomCode(level, itemClass, quality);
    else
        return ItemTrinket::generateRandomCode(level, itemClass, quality);
}

bool Item::fromRandomCodeToValues(QString randomCode, int& level, QMUD::ItemType &itemType, QString &name, QMUD::ItemClassType &classType, QMUD::ItemQualityType &quality)
{
    if (ItemArmor::isValidRandomCode(randomCode))
    {
        itemType = QMUD::ItemType::ARMOR;
        return ItemArmor::fromRandomCodeToValues(randomCode, level, name, classType, quality);
    }
    else if (ItemWeapon::isValidRandomCode(randomCode))
    {
        itemType = QMUD::ItemType::WEAPON;
        return ItemWeapon::fromRandomCodeToValues(randomCode, level, name, classType, quality);
    }
    else if (ItemTrinket::isValidRandomCode(randomCode))
    {
        itemType = QMUD::ItemType::TRINKET;
        return ItemTrinket::fromRandomCodeToValues(randomCode, level, name, classType, quality);
    }
    else
        return false;
}

qint64 Item::jsonToSellValue(const QJsonObject &obj)
{   
    // Value
    double tmpValue;
    if (!QMUD::fileJsonRead(obj, "sellValue", tmpValue, true, 0.0) || tmpValue <= 0.0)
    {
        int level;
        if (!QMUD::fileJsonRead(obj, "level", level))
            return 0;

        QString tmpQualityStr;
        if (!QMUD::fileJsonRead(obj, "quality", tmpQualityStr, false, true, QMUD::itemQualityToString(QMUD::ItemQualityType::JUNK)))
            return 0;

        QMUD::ItemQualityType quality = QMUD::itemQualityFromString(tmpQualityStr);

        return getDefaultCoinsValue(level, quality);
    }

    return qRound64(tmpValue);
}

QString Item::jsonToName(const QJsonObject &obj)
{
    // Value
    QString name;
    if (!QMUD::fileJsonRead(obj, "name", name))
        return "";

    return name;
}

int Item::jsonToLevel(const QJsonObject &obj)
{
    // Value
    int level;
    if (!QMUD::fileJsonRead(obj, "level", level))
        return 1000;

    return level;
}

QMUD::ItemQualityType Item::jsonToQuality(const QJsonObject &obj)
{
    QString tmpQualityStr;
    if (!QMUD::fileJsonRead(obj, "quality", tmpQualityStr, false, true, QMUD::itemQualityToString(QMUD::ItemQualityType::JUNK)))
        return QMUD::ItemQualityType::JUNK;

    return QMUD::itemQualityFromString(tmpQualityStr);
}

void Item::writeJson(QJsonObject &obj)
{
    if (IsModify())
    {
        obj["type"] = QMUD::itemTypeToString(m_eItemType);

        if (!m_strRandomCode.isEmpty())
            obj["randomCode"] = m_strRandomCode;
        else
        {
            obj["id"] = static_cast<double>(m_iStaticId);
            obj["mapId"] = static_cast<double>(m_iMapStaticId);
        }

        obj["name"] = m_strName;
        obj["description"] = QJsonArray::fromStringList(m_strDescription.split("\n"));

        if (m_uuid.isNull())
        {
            m_uuid = QUuid::createUuid();
            GLOBAL_CRITICAL(tr("Trovato un oggetto con UUID non valido: %1:%2:%3, genero %4")
                            .arg(static_cast<double>(m_iStaticId))
                            .arg(static_cast<double>(m_iMapStaticId))
                            .arg(m_strName)
                            .arg(m_uuid.toString()));

        }

        obj["uuid"] = m_uuid.toString();
        if (m_bIsBind)
            obj["isBind"] = m_bIsBind;

        obj["quality"] = QMUD::itemQualityToString(m_eQuality);
    }
    else
    {
        obj["uuid"] = m_uuid.toString();
        if (m_bIsBind)
            obj["isBind"] = m_bIsBind;

        if (!m_strRandomCode.isEmpty())
        {
            obj["randomCode"] = m_strRandomCode;
        }
        else
        {
            obj["id"] = static_cast<double>(m_iStaticId);
            obj["mapId"] = static_cast<double>(m_iMapStaticId);
        }
    }
}

QMUD::ItemType Item::itemType() const
{
    return m_eItemType;
}

QMUD::StaticIdType Item::staticId() const
{
    return m_iStaticId;
}

QMUD::StaticIdType Item::mapStaticId() const
{
    return m_iMapStaticId;
}

QString Item::name(Entity *suitor) const
{
    Q_UNUSED(suitor)

    return m_strName;
}

QString Item::description() const
{
    return m_strDescription;
}

QMUD::ItemQualityType Item::quality() const
{
    return m_eQuality;
}

bool Item::IsModify() const
{
    return false;
}

void Item::destroy()
{
    emit removeFromTheWorld(this);
}

bool Item::isAutoDestroy() const
{
    return m_iAutoDestroyCounter >= 0;
}

void Item::setAutoDestroy(int count)
{
    m_iAutoDestroyCounter = count;
}

int Item::autoDestroyCounter() const
{
    return m_iAutoDestroyCounter;
}

QUuid Item::uuid() const
{
    return m_uuid;
}

bool Item::isBind() const
{
    return m_bIsBind;
}

void Item::setIsBind(bool bind)
{
    m_bIsBind = bind;
}

int Item::level() const
{
    return m_iLevel;
}

qint64 Item::coinsValue() const
{
    return m_iCoinsValue;
}

const QMUD::ItemBaseBonus &Item::baseBonus() const
{
    static QMUD::ItemBaseBonus tmp;
    return tmp;
}

const QMUD::ItemBaseStatus &Item::baseTemporaryStatus() const
{
    static QMUD::ItemBaseStatus tmp;
    return tmp;
}

qint64 Item::getDefaultCoinsValue(int level, QMUD::ItemQualityType quality)
{
    double qualityMultiplier = 0.6;
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:       qualityMultiplier = 0.1;  break;
        case QMUD::ItemQualityType::COMMON:     qualityMultiplier = 0.4;  break;
        case QMUD::ItemQualityType::UNCOMMON:   qualityMultiplier = 1.0;  break;
        case QMUD::ItemQualityType::VALUABLE:   qualityMultiplier = 1.7;  break;
        case QMUD::ItemQualityType::RARE:       qualityMultiplier = 2.4;  break;
        case QMUD::ItemQualityType::EPIC:       qualityMultiplier = 5.0;  break;
        case QMUD::ItemQualityType::LEGENDARY:  qualityMultiplier = 10.0; break;
    }

    return qRound64(level * 20 * qualityMultiplier);
}

int Item::basePrimaryStatBonus(int level)
{
    return qMax(0, qRound((level/4.0) * 1.0));
}

int Item::baseSecondaryStatBonus(int level)
{
    return qMax(0, qRound((level/4.0) * 0.8));
}

int Item::baseTertiaryStatBonus(int level)
{
    return qMax(0, qRound((level/4.0) * 0.6));
}
