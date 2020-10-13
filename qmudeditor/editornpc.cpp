/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "editornpc.h"

#include <QJsonArray>

EditorNpc::EditorNpc(QObject *parent) :
    QObject(parent),
    m_id(0),
    m_name(tr("Nuovo Npc")),
    m_eSex(QMUD::SexType::MALE),
    m_eRace(QMUD::RaceType::HUMAN),
    m_eClass(QMUD::ClassType::MAGE),
    m_eType(QMUD::NpcType::NORMAL),
    m_iLevel(1),
    m_iBonusAttacks(0),
    m_iAssistGroup(0),
    m_bShopEnabled(false),
    m_dShopSaleMultiplier(1.0),
    m_dShopPurchaseMultiplier(1.0),
    m_bShopPurchaseBase(true),
    m_bShopPurchaseArmor(true),
    m_bShopPurchaseWeapon(true)
{

}

bool EditorNpc::read(const QJsonObject &obj)
{
    m_id = obj["id"].toInt();
    m_name = obj["name"].toString();

    m_eSex = QMUD::sexTypeFromString(obj["sex"].toString());
    if (m_eSex == QMUD::SexType::UNKNOWN)
        m_eSex = QMUD::SexType::MALE;

    m_description = obj["description"].toVariant().toStringList().join("\n");
    m_eRace = QMUD::Race::fromString(obj["race"].toString());
    m_eClass = QMUD::classTypeFromString(obj["class"].toString());
    m_eType = QMUD::npcTypeFromString(obj["type"].toString());
    m_iBonusAttacks = obj.contains("numberOfAttacksBonus") ? obj["numberOfAttacksBonus"].toInt() : 0;
    m_iLevel = obj["level"].toInt();

    m_flags.clear();
    auto itFlags = obj.find("flags");

    if (itFlags != obj.end())
    {
        auto array = itFlags->toArray();

        for (auto elem : array)
            m_flags.push_back(QMUD::npcCharacterFlagTypeFromString(elem.toString()));
    }

    m_iAssistGroup = obj["assistGroup"].toInt();

    m_strEventCustom = obj["eventCustom"].toVariant().toStringList().join("\n");
    m_strEventEnter = obj["eventEnter"].toVariant().toStringList().join("\n");
    m_strEventAbility = obj["eventAbility"].toVariant().toStringList().join("\n");

    auto itLoot = obj.find("loot");

    if (itLoot != obj.end())
    {
        auto array = itLoot->toArray();

        for (auto elem : array)
        {
            QJsonObject objItem = elem.toObject();
            LootItemInfo item;
            item.id = objItem["id"].toInt();
            item.rarity = QMUD::itemRarityFromString(objItem["rarity"].toString());
            if (item.rarity == QMUD::ItemRarityType::UNKNOWN)
                item.rarity = QMUD::ItemRarityType::RARITY1;
            m_vLoot.push_back(item);
        }
    }

    auto itShop = obj.find("shop");

    if (itShop != obj.end())
    {
        QJsonObject objShop = itShop->toObject();

        m_bShopEnabled = true;

        QJsonArray arrayList = objShop["list"].toArray();

        for (auto elem : arrayList)
            m_vShopList.push_back(elem.toInt());

        m_dShopSaleMultiplier = objShop["saleInflation"].toDouble();
        m_dShopPurchaseMultiplier = objShop["purchaseInflation"].toDouble();
        m_bShopPurchaseBase = objShop["purchaseBase"].toBool();
        m_bShopPurchaseArmor = objShop["purchaseArmor"].toBool();
        m_bShopPurchaseWeapon = objShop["purchaseWeapon"].toBool();
    }
    else
        m_bShopEnabled = false;

    return true;
}

bool EditorNpc::write(QJsonObject &obj) const
{
    obj["id"] = m_id;
    obj["name"] = m_name;
    obj["sex"] = QMUD::sexTypeToString(m_eSex);

    if (!m_description.isEmpty())
        obj["description"] = QJsonArray::fromStringList(m_description.split("\n"));

    obj["race"] = QMUD::Race::toString(m_eRace);
    obj["class"] = QMUD::classTypeToString(m_eClass);
    obj["type"] = QMUD::npcTypeToString(m_eType);

    if (m_iBonusAttacks > 0)
        obj["numberOfAttacksBonus"] = m_iBonusAttacks;

    obj["level"] = m_iLevel;

    if (!m_flags.isEmpty())
    {
        QJsonArray array;
        for (auto flag : m_flags)
            array.push_back(QMUD::npcCharacterFlagTypeToString(flag));

        obj["flags"] = array;
    }

    if (m_iAssistGroup != 0)
        obj["assistGroup"] = m_iAssistGroup;

    if (!m_strEventCustom.isEmpty())
        obj["eventCustom"] = QJsonArray::fromStringList(m_strEventCustom.split("\n"));

    if (!m_strEventEnter.isEmpty())
        obj["eventEnter"] = QJsonArray::fromStringList(m_strEventEnter.split("\n"));

    if (!m_strEventAbility.isEmpty())
        obj["eventAbility"] = QJsonArray::fromStringList(m_strEventAbility.split("\n"));

    if (!m_vLoot.isEmpty())
    {
        QJsonArray arrayLoot;
        for (auto item : m_vLoot)
        {
            QJsonObject objItem;
            objItem["id"] = item.id;
            objItem["rarity"] = QMUD::itemRarityToString(item.rarity);
            arrayLoot.push_back(objItem);
        }

        obj["loot"] = arrayLoot;
    }

    if (m_bShopEnabled)
    {
        QJsonObject objShop;
        QJsonArray arrayList;

        for (auto elem : m_vShopList)
            arrayList.push_back(elem);

        objShop["list"] = arrayList;
        objShop["saleInflation"] = m_dShopSaleMultiplier;
        objShop["purchaseInflation"] = m_dShopPurchaseMultiplier;
        objShop["purchaseBase"] = m_bShopPurchaseBase;
        objShop["purchaseArmor"] = m_bShopPurchaseArmor;
        objShop["purchaseWeapon"] = m_bShopPurchaseWeapon;

        obj["shop"] = objShop;
    }

    return true;
}

int EditorNpc::id() const
{
    return m_id;
}

void EditorNpc::setId(int id)
{
    m_id = id;
}

QString EditorNpc::name() const
{
    return m_name;
}

void EditorNpc::setName(QString name)
{
    m_name = name;
}

QMUD::SexType EditorNpc::sex() const
{
    return m_eSex;
}

void EditorNpc::setSex(QMUD::SexType type)
{
    m_eSex = type;
}

QString EditorNpc::description() const
{
    return m_description;
}

void EditorNpc::setDescription(QString description)
{
    m_description = description;
}

QMUD::RaceType EditorNpc::race() const
{
    return m_eRace;
}

void EditorNpc::setRace(QMUD::RaceType race)
{
    m_eRace = race;
}

QMUD::ClassType EditorNpc::classType() const
{
    return m_eClass;
}

void EditorNpc::setClassType(QMUD::ClassType c)
{
    m_eClass = c;
}

int EditorNpc::level() const
{
    return m_iLevel;
}

void EditorNpc::setLevel(int level)
{
    m_iLevel = level;
}

QMUD::NpcType EditorNpc::type() const
{
    return m_eType;
}

void EditorNpc::setType(QMUD::NpcType type)
{
    m_eType = type;
}

int EditorNpc::bonusAttacks() const
{
    return m_iBonusAttacks;
}

void EditorNpc::setBonusAttacks(int count)
{
    m_iBonusAttacks = count;
}

QVector<QMUD::NpcCharacterFlagType> EditorNpc::flags() const
{
    return m_flags;
}

void EditorNpc::setFlags(const QVector<QMUD::NpcCharacterFlagType> &flags)
{
    m_flags = flags;
}

int EditorNpc::assistGroup() const
{
    return m_iAssistGroup;
}

void EditorNpc::setAssistGroup(int group)
{
    m_iAssistGroup = group;
}

QString EditorNpc::eventCustom() const
{
    return m_strEventCustom;
}

void EditorNpc::setEventCustom(QString str)
{
    m_strEventCustom = str;
}

QString EditorNpc::eventEnter() const
{
    return m_strEventEnter;
}

void EditorNpc::setEventEnter(QString str)
{
    m_strEventEnter = str;
}

QString EditorNpc::eventAbility() const
{
    return m_strEventAbility;
}

void EditorNpc::setEventAbility(QString str)
{
    m_strEventAbility = str;
}

QVector<EditorNpc::LootItemInfo> EditorNpc::loot() const
{
    return m_vLoot;
}

void EditorNpc::setLoot(QVector<EditorNpc::LootItemInfo> loot)
{
    m_vLoot = loot;
}

bool EditorNpc::shopEnabled() const
{
    return m_bShopEnabled;
}

void EditorNpc::setShopEnabled(bool enabled)
{
    m_bShopEnabled = enabled;
}

QVector<int> EditorNpc::shopList() const
{
    return m_vShopList;
}

void EditorNpc::setShopList(QVector<int> list)
{
    m_vShopList = list;
}

double EditorNpc::shopSaleMultiplier() const
{
    return m_dShopSaleMultiplier;
}

void EditorNpc::setShopSaleMultiplier(double value)
{
    m_dShopSaleMultiplier = value;
}

double EditorNpc::shopPurchaseMultiplier() const
{
    return m_dShopPurchaseMultiplier;
}

void EditorNpc::setShopPurchaseMultiplier(double value)
{
    m_dShopPurchaseMultiplier = value;
}

bool EditorNpc::shopPurchaseBase() const
{
    return m_bShopPurchaseBase;
}

void EditorNpc::setShopPurchaseBase(bool value)
{
    m_bShopPurchaseBase = value;
}

bool EditorNpc::shopPurchaseArmor() const
{
    return m_bShopPurchaseArmor;
}

void EditorNpc::setShopPurchaseArmor(bool value)
{
    m_bShopPurchaseArmor = value;
}

bool EditorNpc::shopPurchaseWeapon() const
{
    return m_bShopPurchaseWeapon;
}

void EditorNpc::setShopPurchaseWeapon(bool value)
{
    m_bShopPurchaseWeapon = value;
}
